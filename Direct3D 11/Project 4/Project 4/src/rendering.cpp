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

Renderer::Renderer(HWND hwindow) : m_loaded(false), m_hwindow(hwindow), m_render_target_view(NULL), m_swap_chain2 (NULL), m_device_context2(NULL), m_device2(NULL),
		m_vertex_shader(NULL), m_pixel_shader(NULL), m_vertex_layout(NULL), m_vertex_buffer(NULL) {
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
	if (m_vertex_buffer) {
		m_vertex_buffer->Release();
	}
	if (m_vertex_layout) {
		m_vertex_layout->Release();
	}
	if (m_vertex_shader) {
		m_vertex_shader->Release();
	}
	if (m_pixel_shader) {
		m_pixel_shader->Release();
	}
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
	ID3D11Device *device = NULL;
	ID3D11DeviceContext *device_context = NULL;
	HRESULT result_device = S_OK;
	for (UINT i = 0; i < _countof(g_driver_types); ++i) {
		m_driver_type = g_driver_types[i];
		result_device = D3D11CreateDevice(
			NULL,						// Default adapter
			m_driver_type,				// Driver type
			NULL,						// A handle to a DLL that implements a software rasterizer.
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
	IDXGIDevice3 *dxgi_device3 = NULL;
	const HRESULT result_dxgi_device3 = m_device2->QueryInterface(__uuidof(IDXGIDevice3), (void **)&dxgi_device3);
	if (FAILED(result_dxgi_device3)) {
		return result_dxgi_device3;
	}
	// Get the IDXGIAdapter.
	IDXGIAdapter *dxgi_adapter = NULL;
	const HRESULT result_dxgi_adapter = dxgi_device3->GetAdapter(&dxgi_adapter);
	if (FAILED(result_dxgi_adapter)) {
		dxgi_device3->Release();

		return result_dxgi_adapter;
	}
	// Get the IDXGIFactory3.
	IDXGIFactory3* dxgi_factory3 = NULL;
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
	const HRESULT result_swap_chain1 = dxgi_factory3->CreateSwapChainForHwnd(m_device2, m_hwindow, &swap_chain_desc, NULL, NULL, &swap_chain1);
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
	ID3D11Texture2D *back_buffer = NULL;
	const HRESULT result_back_buffer = m_swap_chain2->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&back_buffer);
	if (FAILED(result_back_buffer)) {
		return result_back_buffer;
	}
	// Create a ID3D11RenderTargetView.
	const HRESULT result_render_target_view = m_device2->CreateRenderTargetView(back_buffer, NULL, &m_render_target_view);
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
	m_device_context2->OMSetRenderTargets(1, &m_render_target_view, NULL);

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

	// Compile the vertex shader.
	ID3DBlob *vertex_shader_blob = NULL;
	const HRESULT result_vertex_shader_blob = CompileShaderFromFile(L"Project 4/shaders/effect.fx", "VS", "vs_4_0", &vertex_shader_blob);
	if (FAILED(result_vertex_shader_blob)) {
		return result_vertex_shader_blob;
	}
	// Create the vertex shader
	// 1. A pointer to the compiled vertex shader.
	// 2. The size of the compiled vertex shader.
	// 3. A pointer to a class linkage interface.
	// 4. Address of a pointer to a vertex shader.
	const HRESULT result_vertex_shader = m_device2->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &m_vertex_shader);
	if (FAILED(result_vertex_shader)) {
		vertex_shader_blob->Release();
		return result_vertex_shader;
	}

	// Create the ID3D11InputLayout.
	// 1. An array of the input-assembler stage input data types; each type is described by an element description.
	// 2. The number of input-data types in the array of input-elements.
	// 3. A pointer to the compiled shader.
	// 4. The size of the compiled shader.
	// 5.A pointer to the input - layout object created
	const HRESULT result_vertex_layout = m_device2->CreateInputLayout(vertex_input_element_desc, _countof(vertex_input_element_desc), vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &m_vertex_layout);
	// Release the ID3DBlob.
	vertex_shader_blob->Release();
	if (FAILED(result_vertex_layout)) {
		return result_vertex_layout;
	}
	// Set the ID3D11InputLayout.
	m_device_context2->IASetInputLayout(m_vertex_layout);

	// Compile the pixel shader.
	ID3DBlob *pixel_shader_blob = NULL;
	const HRESULT result_pixel_shader_blob = CompileShaderFromFile(L"Project 4/shaders/effect.fx", "PS", "ps_4_0", &pixel_shader_blob);
	if (FAILED(result_pixel_shader_blob)) {
		return result_pixel_shader_blob;
	}
	// Create the pixel shader.
	// 1. A pointer to the compiled pixel shader.
	// 2. The size of the compiled pixel shader.
	// 3. A pointer to a class linkage interface.
	// 4. Address of a pointer to a pixel shader.
	const HRESULT result_pixel_shader = m_device2->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), NULL, &m_pixel_shader);
	// Release the ID3DBlob.
	pixel_shader_blob->Release();
	if (FAILED(result_pixel_shader)) {
		return result_pixel_shader;
	}

	// Create the vertex buffer.
	const HRESULT result_vertex_buffer = InitScene();
	if (FAILED(result_vertex_buffer)) {
		return result_vertex_buffer;
	}
	// Set the vertex buffer.
	UINT stride = sizeof(Vertex);	// The size (in bytes) of the elements that are to be used from a vertex buffer.
	UINT offset = 0;				// The number of bytes between the first element of a vertex buffer and the first element that will be used.
	// 1. The first input slot for binding.
	// 2. The number of vertex buffers in the array.
	// 3. A pointer to an array of vertex buffers.
	// 4. A pointer to an array of stride values.
	// 5. A pointer to an array of offset values.
	m_device_context2->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
	// Bind information about the primitive type, and data order that describes input data for the input assembler stage.
	m_device_context2->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

HRESULT Renderer::InitScene() {
	// Create the vertices.
	const Vertex vertices[] = {
		XMFLOAT3( 0.0f,  0.5f, 0.5f),
		XMFLOAT3( 0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f),
	};
	
	// Describe the buffer resource.
	D3D11_BUFFER_DESC buffer_desc;
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth		= sizeof(vertices);			// Size of the buffer in bytes.
	buffer_desc.Usage			= D3D11_USAGE_DEFAULT;		// How the buffer is expected to be read from and written to.
	buffer_desc.BindFlags		= D3D11_BIND_VERTEX_BUFFER; // How the buffer will be bound to the pipeline.
	buffer_desc.CPUAccessFlags	= 0;						// No CPU access is necessary.
	
	// Specify data for initializing a subresource.
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem			= vertices;					// A pointer to the initialization data.
	
	// Create the vertex buffer.
	// 1. A pointer to a D3D11_BUFFER_DESC structure that describes the buffer.
	// 2. A pointer to a D3D11_SUBRESOURCE_DATA structure that describes the initialization data.
	// 3. Address of a pointer to the ID3D11Buffer interface for the buffer object created.
	return m_device2->CreateBuffer(&buffer_desc, &init_data, &m_vertex_buffer);
}

void Renderer::Render() {
	// A solarized dark background color (some basic colors can be found in <directxcolors.h>)
	const DirectX::XMVECTORF32 background_color = { 0.0f, 0.117647058f, 0.149019608f, 1.000000000f };
	
	// Clear the back buffer
	m_device_context2->ClearRenderTargetView(m_render_target_view, background_color);

	// Set a vertex and pixel shader to the device.
	m_device_context2->VSSetShader(m_vertex_shader, NULL, 0);
	m_device_context2->PSSetShader(m_pixel_shader, NULL, 0);

	// Draw non-indexed, non-instanced primitives.
	// 1. Number of vertices to draw.
	// 2. Index of the first vertex.
	m_device_context2->Draw(3, 0);
	
	// Present the back buffer to the front buffer.
	m_swap_chain2->Present(0, 0);
}

#pragma endregion
