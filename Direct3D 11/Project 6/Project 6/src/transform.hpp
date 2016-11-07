#pragma once

//-----------------------------------------------------------------------------
// Engine Declarations and Definitions
//-----------------------------------------------------------------------------
#pragma region

/**
 A struct of model transforms.
 */
struct ModelTransform {

	/**
	 The model-to-world transform of this model transform.
	 */
	XMMATRIX m_model_to_world;

	/**
	 The world-to-view transform of this model transform.
	 */
	XMMATRIX m_world_to_view;

	/**
	 The view-to-projection transform of this model transform.
	 */
	XMMATRIX m_view_to_projection;
};

#pragma endregion
