#pragma once

//-----------------------------------------------------------------------------
// Engine Definitions and Declarations
//-----------------------------------------------------------------------------
#pragma region

/**
 A struct of (perspective) cameras.
 */
struct Camera {
	
	/**
	 The world to view transform of this camera.
	 */
	XMMATRIX m_world_to_view;

	/**
	 The view to projection transform of this camera.
	 */
	XMMATRIX m_view_to_projection;
};

#pragma endregion