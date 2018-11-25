#include "model.hlsli"

struct VS_INPUT {
	float4 p : POSITION;
	float3 n : NORMAL0;
};

struct VS_OUTPUT {
	float4 p : SV_POSITION;
	float3 n : NORMAL0;
};

VS_OUTPUT VS(VS_INPUT input) {
	VS_OUTPUT output;
	output.p = mul( input.p, g_object_to_world);
	output.p = mul(output.p, g_world_to_camera);
	output.p = mul(output.p, g_camera_to_projection);
	output.n = mul(float4(input.n, 1.0f), g_object_to_world).xyz;
	return output;
}