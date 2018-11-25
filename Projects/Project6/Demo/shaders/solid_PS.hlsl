#include "model.hlsli"

struct PS_INPUT {
	float4 p_viewport : SV_POSITION;
	float3 n          : NORMAL0;
};

float4 PS(PS_INPUT input) : SV_Target{
	return g_color;
}