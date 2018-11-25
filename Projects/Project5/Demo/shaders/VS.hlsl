cbuffer Transform : register(b0) {
	float4x4 g_object_to_world      : packoffset(c0);
	float4x4 g_world_to_camera      : packoffset(c4);
	float4x4 g_camera_to_projection : packoffset(c8);
}

struct VS_INPUT {
	float4 p     : POSITION;
	float4 color : COLOR0;
};

struct VS_OUTPUT {
	float4 p     : SV_POSITION;
	float4 color : COLOR0;
};

VS_OUTPUT VS(VS_INPUT input) {
	VS_OUTPUT output;
	output.p = mul( input.p, g_object_to_world);
	output.p = mul(output.p, g_world_to_camera);
	output.p = mul(output.p, g_camera_to_projection);
	output.color = input.color;
	return output;
}