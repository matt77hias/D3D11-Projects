#include "model.hlsli"

struct PS_INPUT {
	float4 p_viewport : SV_POSITION;
	float3 n          : NORMAL0;
};

float4 PS(PS_INPUT input) : SV_Target {
	const float3 n = normalize(input.n);
	
	float3 c = 0.0f;
	for (uint i = 0u; i < 2u; ++i) {
		c += dot(g_d_lights[i].xyz, n) * g_color_lights[i].xyz;
	}

	return float4(saturate(c), 1.0f);
}