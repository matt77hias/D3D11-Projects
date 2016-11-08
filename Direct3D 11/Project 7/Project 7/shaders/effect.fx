//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------
Texture2D diffuse_texture : register(t0);
SamplerState linear_sample : register(s0);

cbuffer cb_never_changes : register(b0) {
	matrix world_to_view;
};

cbuffer cb_change_on_resize : register(b1) {
	matrix view_to_projection;
};

cbuffer cb_changes_every_frame : register(b2) {
	matrix model_to_world;
	float4 mesh_color;
}

//-----------------------------------------------------------------------------
// Input structures
//-----------------------------------------------------------------------------
struct VS_INPUT {
	float4 p : POSITION;
	float2 tex : TEXCOORD0;
};

struct PS_INPUT {
	float4 p : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) {
	PS_INPUT output = (PS_INPUT)0;
	output.p = mul(input.p, model_to_world);
	output.p = mul(output.p, world_to_view);
	output.p = mul(output.p, view_to_projection);
	output.tex = input.tex;
	return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target {
	return diffuse_texture.Sample(linear_sample, input.tex) * mesh_color;
}