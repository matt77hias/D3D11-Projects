#pragma once

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#pragma region

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

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
	Vertex() : p(XMFLOAT3(0.0f, 0.0f, 0.0f)), diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)) {}

	/**
	 Constructs a vertex.

	 @param[in]		p
					Position of the vertex (in object space).
	 @param[in]		diffuse
					Diffuse color of the vertex.
	 */
	Vertex(XMFLOAT3 p, XMFLOAT4 diffuse) : p(p), diffuse(diffuse) {}

	/**
	 Position of this vertex (in object space).
	 */
	XMFLOAT3 p;

	/**
	 Diffuse color of this vertex.
	 */
	XMFLOAT4 diffuse;
};

// 1. The HLSL semantic associated with this element in a shader input-signature.
// 2. The semantic index for the element.
// 3. The data type of the element data.
// 4. An integer value that identifies the input assembler.
// 5. Offset(in bytes) between each element.
// 6. Identifies the input data class for a single input slot.
// 7. The number of instances to draw using the same per instance data before advancing in the buffer by one element.
const D3D11_INPUT_ELEMENT_DESC vertex_input_element_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, UINT(offsetof(Vertex, p)),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, UINT(offsetof(Vertex, diffuse)), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

#pragma endregion