#pragma once

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#pragma region

using DirectX::XMFLOAT3;

#pragma endregion

//-----------------------------------------------------------------------------
// Engine Declarations and Definitions
//-----------------------------------------------------------------------------
#pragma region

/**
 A struct of vertices.
 */
struct Vertex final {

public:

	/**
	 Constructs a vertex.
	 */
	Vertex() : p(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)) {}

	/**
	 Constructs a vertex.

	 @param[in]		p
					Position of the vertex (in world space).
	 */
	Vertex(XMFLOAT3 p) : p(p) {}

	/**
	 Position of this vertex (in world space).
	 */
	XMFLOAT3 p;
};

// 1. The HLSL semantic associated with this element in a shader input-signature.
// 2. The semantic index for the element.
// 3. The data type of the element data.
// 4. An integer value that identifies the input assembler.
// 5. Offset(in bytes) between each element.
// 6. Identifies the input data class for a single input slot.
// 7. The number of instances to draw using the same per instance data before advancing in the buffer by one element.
const D3D11_INPUT_ELEMENT_DESC vertex_input_element_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, UINT(offsetof(Vertex, p)), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

#pragma endregion