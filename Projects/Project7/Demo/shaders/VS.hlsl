#include "buffer.hlsli"

struct VS_INPUT {
	float4 p   : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT {
	float4 p   : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input) {
	VS_OUTPUT output;
	output.p   = mul( input.p, object_to_world);
	output.p   = mul(output.p, world_to_camera);
	output.p   = mul(output.p, camera_to_projection);
	output.tex = input.tex;
	return output;
}