//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#pragma region

#include <SDKDDKVer.h>
#include "exception\exception.hpp"
#include "logging\dump.hpp"
#include "system\timer.hpp"
#include "ui\window.hpp"

#pragma endregion

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#pragma region

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <dxgi1_2.h>

#pragma endregion

//-----------------------------------------------------------------------------
// Linker Directives
//-----------------------------------------------------------------------------
#pragma region

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxgi.lib")

#pragma endregion

//-----------------------------------------------------------------------------
// Integrated + Dedicated GPU on notebooks
//-----------------------------------------------------------------------------
#pragma region

/**
 NVIDIA Optimus enablement
 @pre NVIDIA Control Panel > Preferred graphics processor > "Auto-select"
 */
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

/**
 AMD "Optimus" enablement
 */
extern "C" {
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#pragma endregion

//-----------------------------------------------------------------------------
// Declarations and Definitions
//-----------------------------------------------------------------------------
using namespace mage;

namespace {

	constexpr U32x2 g_display_resolution = { 800u, 600u };
	UniquePtr< Window > g_window;

	constexpr D3D_DRIVER_TYPE g_driver_types[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	constexpr D3D_FEATURE_LEVEL g_feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_DRIVER_TYPE g_driver_type;
	D3D_FEATURE_LEVEL g_feature_level;
	ComPtr< ID3D11Device > g_device;
	ComPtr< ID3D11DeviceContext > g_device_context;
	ComPtr< IDXGISwapChain1 > g_swap_chain;
	ComPtr< ID3D11RenderTargetView > g_rtv;
	ComPtr< ID3D11DepthStencilView > g_dsv;

	ComPtr< ID3D11VertexShader > g_vs;
	ComPtr< ID3D11PixelShader > g_ps;
	ComPtr< ID3D11InputLayout > g_vertex_layout;
	ComPtr< ID3D11Buffer > g_vertex_buffer;
	ComPtr< ID3D11Buffer > g_index_buffer;
	ComPtr< ID3D11Buffer > g_transform_buffer;
	
	struct Vertex {
		F32x3 m_p;
		F32x4 m_color;
		static const D3D11_INPUT_ELEMENT_DESC s_input_element_descs[2u];
	};

	static_assert(28u == sizeof(Vertex), "Vertex struct/layout mismatch");

	const D3D11_INPUT_ELEMENT_DESC Vertex::s_input_element_descs[] = { 
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT,    0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u },
	    { "COLOR",    0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u } 
	};

	struct alignas(16) Transform {
		DirectX::XMMATRIX m_object_to_world;
		DirectX::XMMATRIX m_world_to_camera;
		DirectX::XMMATRIX m_camera_to_projection;
	};

	Transform g_camera;

	void Init(NotNull< HINSTANCE > instance) {
		#ifdef _DEBUG
		const int debug_flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		// Perform automatic leak checking at program exit through a call to
		// _CrtDumpMemoryLeaks and generate an error report if the application
		// failed to free all the memory it allocated.
		_CrtSetDbgFlag(debug_flags | _CRTDBG_LEAK_CHECK_DF);
		#endif

		// Add filter for unhandled exceptions.
		AddUnhandledExceptionFilter();

		// Initialize a console.
		InitializeConsole();
		Print("Copyright (c) 2016 Matthias Moulin.\n");

		// Initialize a window.
		{
			auto window_desc = MakeShared< WindowDescriptor >(instance, L"Demo");
			g_window = MakeUnique< Window >(std::move(window_desc),
											L"Demo",
											g_display_resolution);
		}

		// Initialize the device and device context.
		{
			#ifdef NDEBUG
			constexpr U32 create_device_flags = 0u;
			#else  // NDEBUG
			constexpr U32 create_device_flags = D3D11_CREATE_DEVICE_DEBUG;
			#endif // NDEBUG

			// Get the ID3D11Device and ID3D11DeviceContext.
			HRESULT result = E_FAIL;
			for (const auto driver_type : g_driver_types) {
				g_driver_type = driver_type;
				result = D3D11CreateDevice(nullptr,
										   g_driver_type,
										   nullptr,
										   create_device_flags,
										   g_feature_levels,
										   static_cast< U32 >(std::size(g_feature_levels)),
										   D3D11_SDK_VERSION,
										   g_device.ReleaseAndGetAddressOf(),
										   &g_feature_level,
										   g_device_context.ReleaseAndGetAddressOf());
				if (SUCCEEDED(result)) {
					break;
				}
			}
			ThrowIfFailed(result, "ID3D11Device creation failed: {:08X}.", result);
		}
	
		// Initialize the swap chain.
		{
			// Get the IDXGIDevice2.
			ComPtr< IDXGIDevice2 > dxgi_device;
			{
				const HRESULT result = g_device->QueryInterface(__uuidof(IDXGIDevice2), 
					                                            (void**)dxgi_device.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "IDXGIDevice2 creation failed: {:08X}.", result);
			}

			// Get the IDXGIAdapter.
			ComPtr< IDXGIAdapter > dxgi_adapter;
			{
				const HRESULT result = dxgi_device->GetAdapter(dxgi_adapter.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "IDXGIAdapter creation failed: {:08X}.", result);
			}

			// Get the IDXGIFactory2.
			ComPtr< IDXGIFactory2 > dxgi_factory;
			{
				const HRESULT result = dxgi_adapter->GetParent(__uuidof(IDXGIFactory2),
					                                           (void**)dxgi_factory.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "IDXGIFactory2 creation failed: {:08X}.", result);
			}

			dxgi_factory->MakeWindowAssociation(g_window->GetWindow(),
												DXGI_MWA_NO_WINDOW_CHANGES
			                                  | DXGI_MWA_NO_ALT_ENTER
			                                  | DXGI_MWA_NO_PRINT_SCREEN);

			// Create the swap chain descriptor.
			DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
			swap_chain_desc.Width       = g_display_resolution[0u];
			swap_chain_desc.Height      = g_display_resolution[1u];
			swap_chain_desc.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_desc.SampleDesc.Count = 1u;
			swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_desc.BufferCount = 1u;
			swap_chain_desc.AlphaMode   = DXGI_ALPHA_MODE_IGNORE;
			swap_chain_desc.Flags       = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			// Create the swap chain.
			{
				const HRESULT result = dxgi_factory->CreateSwapChainForHwnd(g_device.Get(),
																			g_window->GetWindow(),
																			&swap_chain_desc,
																			nullptr,
																			nullptr,
																			g_swap_chain.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "IDXGISwapChain creation failed: {:08X}.", result);
			}
		
			g_swap_chain->SetFullscreenState(FALSE, nullptr);
		}
	
		// Initialize the RTV and DSV.
		{
			// Get the back buffer.
			ComPtr< ID3D11Texture2D > back_buffer;
			{
				const HRESULT result = g_swap_chain->GetBuffer(0u,
															   __uuidof(ID3D11Texture2D),
															   (void**)back_buffer.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "Back buffer texture creation failed: {:08X}.", result);
			}

			// Create the RTV.
			{
				// Create the RTV.
				const HRESULT result = g_device->CreateRenderTargetView(back_buffer.Get(),
																		nullptr,
																		g_rtv.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "Back buffer RTV creation failed: {:08X}.", result);
			}
		
			// Create the depth stencil texture.
			ComPtr< ID3D11Texture2D > depth_stencil_texture;
			{
				// Create the depth stencil texture.
				D3D11_TEXTURE2D_DESC depth_stencil_desc = {};
				depth_stencil_desc.Width     = g_display_resolution[0u];
				depth_stencil_desc.Height    = g_display_resolution[1u];
				depth_stencil_desc.MipLevels = 1u;
				depth_stencil_desc.ArraySize = 1u;
				depth_stencil_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depth_stencil_desc.SampleDesc.Count   = 1u;
				depth_stencil_desc.SampleDesc.Quality = 0u;
				depth_stencil_desc.Usage     = D3D11_USAGE_DEFAULT;
				depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

				const HRESULT result = g_device->CreateTexture2D(&depth_stencil_desc, nullptr,
																 depth_stencil_texture.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "Depth stencil texture creation failed: {:08X}.", result);
			}

			// Create the depth stencil view.
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
				dsv_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsv_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
				dsv_desc.Texture2D.MipSlice = 0u;
				const HRESULT result = g_device->CreateDepthStencilView(depth_stencil_texture.Get(), &dsv_desc, g_dsv.ReleaseAndGetAddressOf());
			}

			// Bind the RTV and DSV.
			g_device_context->OMSetRenderTargets(1u, g_rtv.GetAddressOf(), g_dsv.Get());
		}
	
		// Initialize the viewport.
		{
			// Create the viewport.
			D3D11_VIEWPORT viewport;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			viewport.Width    = static_cast< F32 >(g_display_resolution[0u]);
			viewport.Height   = static_cast< F32 >(g_display_resolution[1u]);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			// Bind the viewport.
			g_device_context->RSSetViewports(1u, &viewport);
		}
	
		// Initialize the vertex shader.
		{
			// Read the compiled vertex shader blob from file.
			ComPtr< ID3DBlob > shader_blob;
			{
				const HRESULT result = D3DReadFileToBlob(L"VS.cso", shader_blob.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "Shader blob reading failed: {:08X}.", result);
			}

			// Create the vertex shader.
			const HRESULT result = g_device->CreateVertexShader(shader_blob->GetBufferPointer(),
																shader_blob->GetBufferSize(), 
																nullptr,
																g_vs.ReleaseAndGetAddressOf());
			ThrowIfFailed(result, "Vertex shader creation failed: {:08X}.", result);

			// Bind the vertex shader.
			g_device_context->VSSetShader(g_vs.Get(), nullptr, 0u);

			// Create the vertex layout.
			const HRESULT result2 = g_device->CreateInputLayout(Vertex::s_input_element_descs,
																static_cast< U32 >(std::size(Vertex::s_input_element_descs)),
															    shader_blob->GetBufferPointer(),
																shader_blob->GetBufferSize(),
																g_vertex_layout.ReleaseAndGetAddressOf());
			ThrowIfFailed(result2, "Vertex layout creation failed: {:08X}.", result2);

			// Bind the vertex layout.
			g_device_context->IASetInputLayout(g_vertex_layout.Get());
		}

		// Initialize the pixel shader.
		{
			// Read the compiled pixel shader blob from file.
			ComPtr< ID3DBlob > shader_blob;
			{
				const HRESULT result = D3DReadFileToBlob(L"PS.cso", shader_blob.ReleaseAndGetAddressOf());
				ThrowIfFailed(result, "Shader blob reading failed: {:08X}.", result);
			}

			// Create the pixel shader.
			const HRESULT result = g_device->CreatePixelShader(shader_blob->GetBufferPointer(),
															   shader_blob->GetBufferSize(),
															   nullptr,
															   g_ps.ReleaseAndGetAddressOf());
			ThrowIfFailed(result, "Pixel shader creation failed: {:08X}.", result);

			// Bind the pixel shader.
			g_device_context->PSSetShader(g_ps.Get(), nullptr, 0u);
		}
	
		// Initialize the vertex buffer.
		{
			// Create the vertices.
			static constexpr Vertex vertices[] = {
				{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
				{ {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
				{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
				{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
				{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
				{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },
				{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
				{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }
			};

			// Describe the buffer resource.
			D3D11_BUFFER_DESC buffer_desc = {};
			buffer_desc.ByteWidth = sizeof(vertices);
			buffer_desc.Usage     = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			// Specify data for initializing a subresource.
			D3D11_SUBRESOURCE_DATA init_data = {};
			init_data.pSysMem     = vertices;

			const HRESULT result = g_device->CreateBuffer(&buffer_desc,
														  &init_data,
														  g_vertex_buffer.ReleaseAndGetAddressOf());
			ThrowIfFailed(result, "Vertex buffer creation failed: {:08X}.", result);

			// Bind the vertex buffer.
			static constexpr U32 stride = sizeof(Vertex);
			static constexpr U32 offset = 0u;
			g_device_context->IASetVertexBuffers(0u, 1u, g_vertex_buffer.GetAddressOf(),
												 &stride, &offset);
			g_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		// Initialize the index buffer.
		{
			// Create the indices.
			static constexpr U8 indices[] = {
				3u, 1u, 0u,
				2u, 1u, 3u,
				0u, 5u, 4u,
				1u, 5u, 0u,
				3u, 4u, 7u,
				0u, 4u, 3u,
				1u, 6u, 5u,
				2u, 6u, 1u,
				2u, 7u, 6u,
				3u, 7u, 2u,
				6u, 4u, 5u,
				7u, 4u, 6u,
			};

			// Describe the buffer resource.
			D3D11_BUFFER_DESC buffer_desc = {};
			buffer_desc.ByteWidth = sizeof(indices);
			buffer_desc.Usage     = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			// Specify data for initializing a subresource.
			D3D11_SUBRESOURCE_DATA init_data = {};
			init_data.pSysMem     = indices;

			const HRESULT result = g_device->CreateBuffer(&buffer_desc,
														  &init_data,
														  g_index_buffer.ReleaseAndGetAddressOf());
			ThrowIfFailed(result, "Vertex buffer creation failed: {:08X}.", result);

			// Bind the index buffer.
			g_device_context->IASetIndexBuffer(g_index_buffer.Get(), DXGI_FORMAT_R8_UINT, 0u);
		}
	
		// Initialize the transform buffer.
		{
			// Describe the buffer resource.
			D3D11_BUFFER_DESC buffer_desc = {};
			buffer_desc.ByteWidth = sizeof(Transform);
			buffer_desc.Usage     = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			const HRESULT result = g_device->CreateBuffer(&buffer_desc,
														  nullptr,
														  g_transform_buffer.ReleaseAndGetAddressOf());
			ThrowIfFailed(result, "Transform buffer creation failed: {:08X}.", result);

			// Bind the transform buffer.
			g_device_context->VSSetConstantBuffers(0u, 1u, g_transform_buffer.GetAddressOf());
		}

		// Initialize the camera.
		{
			using namespace DirectX;

			g_camera.m_object_to_world = XMMatrixIdentity();
			static const XMVECTOR p_eye   = { 0.0f, 4.0f, -10.0f, 0.0f };
			static const XMVECTOR p_focus = { 0.0f, 1.0f,  0.0f, 0.0f };
			static const XMVECTOR d_up    = { 0.0f, 1.0f,  0.0f, 0.0f };
			g_camera.m_world_to_camera = XMMatrixLookAtLH(p_eye, p_focus, d_up);
			const F32 aspect_ratio = g_display_resolution[0u] 
				                     / static_cast< F32 >(g_display_resolution[1u]);
			g_camera.m_camera_to_projection 
				= XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, aspect_ratio, 0.01f, 100.0f);
		}
	}

	void Uninit() noexcept {
		// Switch to windowed mode since Direct3D is incapable of when in 
		// fullscreen mode due to certain threading issues that occur behind
		// the scenes.
		if (g_swap_chain) {
			g_swap_chain->SetFullscreenState(FALSE, nullptr);
		}

		// Reset any device context to the default settings.
		if (g_device_context) {
			g_device_context->ClearState();
		}
	}
	
	void Render(F64 time) {
		static constexpr F32x4 s_background_color 
			= { 0.0f, 0.117647058f, 0.149019608f, 1.0f };

		// Clear the back buffer.
		g_device_context->ClearRenderTargetView(g_rtv.Get(), s_background_color.data());
		// Clear the depth buffer to 1.0 (i.e. max depth).
		g_device_context->ClearDepthStencilView(g_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

		using namespace DirectX;

		// Animate the first cube.
		const XMMATRIX object_to_world1 = XMMatrixRotationY(5.0f * static_cast< F32 >(time));
		// Animate the second cube.
		const XMMATRIX spin      = XMMatrixRotationZ(-static_cast< F32 >(time));
		const XMMATRIX orbit     = XMMatrixRotationY(2.0f * static_cast< F32 >(time));
		const XMMATRIX translate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
		const XMMATRIX scale     = XMMatrixScaling(0.3f, 0.3f, 0.3f);
		const XMMATRIX object_to_world2 = scale * spin * translate * orbit;

		// Draw the first cube.
		Transform transform;
		transform.m_object_to_world      = XMMatrixTranspose(object_to_world1);
		transform.m_world_to_camera      = XMMatrixTranspose(g_camera.m_world_to_camera);
		transform.m_camera_to_projection = XMMatrixTranspose(g_camera.m_camera_to_projection);
		g_device_context->UpdateSubresource(g_transform_buffer.Get(), 0u, nullptr, &transform, 0u, 0u);
		g_device_context->DrawIndexed(36u, 0u, 0u);

		// Draw the second cube.
		Transform transform2;
		transform2.m_object_to_world      = XMMatrixTranspose(object_to_world2);
		transform2.m_world_to_camera      = XMMatrixTranspose(g_camera.m_world_to_camera);
		transform2.m_camera_to_projection = XMMatrixTranspose(g_camera.m_camera_to_projection);
		g_device_context->UpdateSubresource(g_transform_buffer.Get(), 0u, nullptr, &transform2, 0u, 0u);
		g_device_context->DrawIndexed(36u, 0u, 0u);

		// Present the back buffer to the front buffer.
		g_swap_chain->Present(0u, 0u);
	}

	[[nodiscard]]
	int Run(int nCmdShow) {
		// Show the main window.
		g_window->Show(nCmdShow);

		CPUTimer timer;
		timer.Start();

		// Enter the message loop.
		MSG msg;
		SecureZeroMemory(&msg, sizeof(msg));
		while (WM_QUIT != msg.message) {

			// Retrieves messages for any window that belongs to the current
			// thread without performing range filtering. Furthermore messages
			// are removed after processing.
			if (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
				// Translates virtual-key messages into character messages.
				TranslateMessage(&msg);
				// Dispatches a message to a window procedure.
				DispatchMessage(&msg);
				continue;
			}

			Render(timer.GetTotalDeltaTime().count());
		}

		Uninit();
		
		return static_cast< int >(msg.wParam);
	}
}

int WINAPI WinMain(_In_ HINSTANCE instance,
				   _In_opt_ [[maybe_unused]] HINSTANCE prev_instance,
				   _In_     [[maybe_unused]] LPSTR lpCmdLine,
				   _In_ int nCmdShow) {

	Init(NotNull< HINSTANCE >(instance));
	return Run(nCmdShow);
}
