#include "stdafx.h"

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#pragma region

#include <directxcolors.h>

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
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

/**
 AMD "Optimus" enablement
 */
extern "C" {
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Definitions
//-----------------------------------------------------------------------------
#pragma region

Renderer::Renderer(HWND hwindow) : m_loaded(false), m_hwindow(hwindow), m_render_target_view(nullptr), m_swap_chain2 (nullptr), m_device_context2(nullptr), m_device2(nullptr) {
	const HRESULT result_init = InitDevice();
	if (FAILED(result_init)) {
		return;
	}

	m_loaded = true;
}

Renderer::~Renderer() {

	// Reset any device context to the default settings. 
	if (m_device_context2) {
		m_device_context2->ClearState();
	}

	// Release D3D11 components
	if (m_render_target_view) { 
		m_render_target_view->Release(); 
	}
	if (m_swap_chain2) { 
		m_swap_chain2->Release(); 
	}
	if (m_device_context2) { 
		m_device_context2->Release(); 
	}
	if (m_device2) { 
		m_device2->Release(); 
	}
}

HRESULT Renderer::InitDevice() {

	// Set the runtime layers to enable.
	UINT create_device_flags = 0;
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Get the ID3D11Device and ID3D11DeviceContext.
	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *device_context = nullptr;
	HRESULT result_device = S_OK;
	for (UINT i = 0; i < _countof(g_driver_types); ++i) {
		m_driver_type = g_driver_types[i];
		result_device = D3D11CreateDevice(
			nullptr,						// Default adapter
			m_driver_type,				// Driver type
			nullptr,						// A handle to a DLL that implements a software rasterizer.
			create_device_flags,		// The runtime layers to enable.
			g_feature_levels,			// The order of feature levels to attempt to create.
			_countof(g_feature_levels),	// The number of feature levels.
			D3D11_SDK_VERSION,			// The SDK version.
			&device,					// The address of a pointer to the ID3D11Device that represents the device created.
			&m_feature_level,			// The address of a pointer to the supported feature level.
			&device_context			// The address of a pointer to the ID3D11DeviceContext.
		);
		if (SUCCEEDED(result_device)) {
			break;
		}
	}

	if (FAILED(result_device)) {
		return result_device;
	}

	// Get the ID3D11Device2 and ID3D11DeviceContext2.
	const HRESULT result_device2 = device->QueryInterface(__uuidof(ID3D11Device2), (void **)&m_device2);
	const HRESULT result_device_context2 = device_context->QueryInterface(__uuidof(ID3D11DeviceContext2), (void **)&m_device_context2);
	// Release the ID3D11Device and ID3D11DeviceContext.
	device->Release();
	device_context->Release();

	if (FAILED(result_device2)) {
		return result_device2;
	}
	if (FAILED(result_device_context2)) {
		return result_device_context2;
	}

	// Get the IDXGIDevice3.
	IDXGIDevice3 *dxgi_device3 = nullptr;
	const HRESULT result_dxgi_device3 = m_device2->QueryInterface(__uuidof(IDXGIDevice3), (void **)&dxgi_device3);
	if (FAILED(result_dxgi_device3)) {
		return result_dxgi_device3;
	}
	// Get the IDXGIAdapter.
	IDXGIAdapter *dxgi_adapter = nullptr;
	const HRESULT result_dxgi_adapter = dxgi_device3->GetAdapter(&dxgi_adapter);
	if (FAILED(result_dxgi_adapter)) {
		dxgi_device3->Release();

		return result_dxgi_adapter;
	}
	// Get the IDXGIFactory3.
	IDXGIFactory3* dxgi_factory3 = nullptr;
	const HRESULT result_dxgi_factory3 = dxgi_adapter->GetParent(__uuidof(IDXGIFactory3), (void **)&dxgi_factory3);
	if (FAILED(result_dxgi_factory3)) {
		dxgi_adapter->Release();
		dxgi_device3->Release();

		return result_dxgi_factory3;
	}
	// Release the IDXGIDevice3 and IDXGIAdapter.
	dxgi_adapter->Release();
	dxgi_device3->Release();

	// Get client rectangle.
	RECT client_rectangle;
	GetClientRect(m_hwindow, &client_rectangle);
	const UINT width  = client_rectangle.right  - client_rectangle.left;
	const UINT height = client_rectangle.bottom - client_rectangle.top;

	// Prepare the swap-chain.
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.Width				= width;							// The resolution width.
	swap_chain_desc.Height				= height;							// The resolution height.
	swap_chain_desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;		// Display format.
	swap_chain_desc.SampleDesc.Count	= 1;								// The number of multisamples per pixel.
	swap_chain_desc.SampleDesc.Quality	= 0;								// The image quality level. (lowest)
	swap_chain_desc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;	// Use the surface or resource as an output render target.
	swap_chain_desc.BufferCount			= 1;								// The number of buffers in the swap chain.

	// Get the IDXGISwapChain1.
	IDXGISwapChain1 *swap_chain1;
	const HRESULT result_swap_chain1 = dxgi_factory3->CreateSwapChainForHwnd(m_device2, m_hwindow, &swap_chain_desc, nullptr, nullptr, &swap_chain1);
	if (FAILED(result_swap_chain1)) {
		return result_swap_chain1;
	}
	// Get the IDXGISwapChain2.
	const HRESULT result_swap_chain2 = swap_chain1->QueryInterface(__uuidof(IDXGISwapChain2), (void **)&m_swap_chain2);
	if (FAILED(result_swap_chain2)) {
		swap_chain1->Release();

		return result_swap_chain2;
	}
	// Release the IDXGISwapChain1.
	swap_chain1->Release();
	
	// We support no full-screen swap-chains so the ALT+ENTER shortcut is blocked
	dxgi_factory3->MakeWindowAssociation(m_hwindow, DXGI_MWA_NO_ALT_ENTER);
	// Release the IDXGIFactory3.
	dxgi_factory3->Release();

	// Access the only back buffer of the swap-chain's.
	ID3D11Texture2D *back_buffer = nullptr;
	const HRESULT result_back_buffer = m_swap_chain2->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&back_buffer);
	if (FAILED(result_back_buffer)) {
		return result_back_buffer;
	}
	// Create a ID3D11RenderTargetView.
	const HRESULT result_render_target_view = m_device2->CreateRenderTargetView(back_buffer, nullptr, &m_render_target_view);
	// Release the back buffer.
	back_buffer->Release();
	if (FAILED(result_render_target_view)) {
		return result_render_target_view;
	}

	// Bind one or more render targets atomically and 
	// the depth-stencil buffer to the output-merger stage.
	// 1. Number of render targets to bind.
	// 2. Pointer to an array of ID3D11RenderTargetViews
	// 3. The depth-stencil state is not bound.
	m_device_context2->OMSetRenderTargets(1, &m_render_target_view, nullptr);

	// Setup the (default) viewport
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;					// x position of the left hand side of the viewport.
	viewport.TopLeftY = 0;					// y position of the top of the viewport.
	viewport.Width		= (FLOAT)width;		// Width of the viewport.
	viewport.Height		= (FLOAT)height;	// Height of the viewport.
	viewport.MinDepth	= 0.0f;				// Minimum depth of the viewport.
	viewport.MaxDepth	= 1.0f;				// Maximum depth of the viewport.
	
	// Bind an array of viewports to the rasterizer stage of the pipeline.
	// 1. Number of viewports to bind.
	// 2. An array of D3D11_VIEWPORT structures to bind to the device.
	m_device_context2->RSSetViewports(1, &viewport);

	return S_OK;
}

void Renderer::Render() {
	// A solarized dark background color (some basic colors can be found in <directxcolors.h>)
	const DirectX::XMVECTORF32 background_color = { 0.0f, 0.117647058f, 0.149019608f, 1.000000000f };
	
	// Clear the back buffer.
	m_device_context2->ClearRenderTargetView(m_render_target_view, background_color);
	
	// Present the back buffer to the front buffer.
	m_swap_chain2->Present(0, 0);
}

#pragma endregion
