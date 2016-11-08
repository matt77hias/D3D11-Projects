//--------------------------------------------------------------------------------------
// ModelTransform Variables
//--------------------------------------------------------------------------------------
cbuffer ModelTransform : register(b0) {
	matrix model_to_world;
	matrix world_to_view;
	matrix view_to_projection;
	float4 d_lights[2];
	float4 color_lights[2];
	float4 output_color;
}

//-----------------------------------------------------------------------------
// Input structures
//-----------------------------------------------------------------------------
struct VS_INPUT {
	float4 p : POSITION;
	float3 n : NORMAL;
};

struct PS_INPUT {
	float4 p : SV_POSITION;
	float3 n : TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) {
	PS_INPUT output = (PS_INPUT)0;
	output.p = mul(input.p, model_to_world);
	output.p = mul(output.p, world_to_view);
	output.p = mul(output.p, view_to_projection);
	output.n = mul(float4(input.n, 1), model_to_world).xyz;
	return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader - With lighting
//-----------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target {
	float4 final_color = 0;

	//do NdotL lighting for 2 lights
	for (int i = 0; i < 2; ++i) {
		final_color += saturate(dot((float3)d_lights[i], input.n) * color_lights[i]);
	}
	final_color.a = 1;
	return final_color;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - With ambient lighting
//--------------------------------------------------------------------------------------
float4 PSSolid(PS_INPUT input) : SV_Target {
	return output_color;
}