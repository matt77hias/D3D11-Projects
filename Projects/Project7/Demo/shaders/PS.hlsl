#include "buffer.hlsli"

Texture2D    g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PS_INPUT {
	float4 p_viewport : SV_POSITION;
	float2 tex        : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target {
	return g_texture.Sample(g_sampler, input.tex) * color;
}