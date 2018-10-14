#pragma once

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
	explicit Renderer(HWND hwindow);

	/**
	 Destructs this renderer.
	 */
	~Renderer();

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
};

#pragma endregion
