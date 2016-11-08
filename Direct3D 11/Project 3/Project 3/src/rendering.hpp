#pragma once

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#pragma region

#include "shader_utils.hpp"

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Defines
//-----------------------------------------------------------------------------
#pragma region

#ifdef _DEBUG
#define ENGINE_COMPILE_SHADERS
#endif

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Definitions and Declarations
//-----------------------------------------------------------------------------
#pragma region

/**
 The supported driver types.
 */
const D3D_DRIVER_TYPE g_driver_types[] = {
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE,
};

/**
 The supported feature levels.
 */
const D3D_FEATURE_LEVEL g_feature_levels[] = {
	D3D_FEATURE_LEVEL_11_1
};

/**
 A class of renderers.
 */
class Renderer {

public:

	/**
	 Constructs a renderer.

	 @param[in]		The main window handle.
	 */
	Renderer(HWND hwindow);

	/**
	 Destructs this renderer.
	 */
	virtual ~Renderer();

	/**
	 Checks whether this renderer is loaded.

	 @return		@c true if this renderer is loaded.
					@c false otherwise.
	 */
	bool IsLoaded() const {
		return m_loaded;
	}

	/**
	 Renders the current frame.
	 */
	void Render();

private:

	/**
	 Initializes the D3D11 device of this renderer.

	 @return		A success/error value.
	 */
	HRESULT InitDevice();

	/**
	 Initializes the scene of this renderer.

	 @return		A success/error value.
	 */
	HRESULT InitScene();

	/**
	 Main window handle of this renderer.
	 */
	HWND m_hwindow;

	/**
	 Flag indicating wether this renderer is loaded.
	 */
	bool m_loaded;

	D3D_DRIVER_TYPE         m_driver_type;
	D3D_FEATURE_LEVEL		m_feature_level;
	ID3D11Device2          *m_device2;
	ID3D11DeviceContext2   *m_device_context2;
	IDXGISwapChain2        *m_swap_chain2;
	ID3D11RenderTargetView *m_render_target_view;

	ID3D11VertexShader     *m_vertex_shader;
	ID3D11PixelShader      *m_pixel_shader;
	ID3D11InputLayout      *m_vertex_layout;
	ID3D11Buffer           *m_vertex_buffer;
};

#pragma endregion