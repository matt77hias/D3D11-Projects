//--------------------------------------------------------------------------------------
// ModelTransform Variables
//--------------------------------------------------------------------------------------
cbuffer ModelTransform : register(b0) {
	matrix model_to_world;
	matrix world_to_view;
	matrix view_to_projection;
}

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
struct VS_OUTPUT {
	float4 p : SV_POSITION;
	float4 diffuse : COLOR0;
};

VS_OUTPUT VS(float4 p : POSITION, float4 diffuse : COLOR) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.p = mul(p, model_to_world);
	output.p = mul(output.p, world_to_view);
	output.p = mul(output.p, view_to_projection);
	output.diffuse = diffuse;
	return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target {
	return input.diffuse;
}