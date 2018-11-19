cbuffer Model : register(b0) {
	float4x4 object_to_world;
	float4x4 world_to_camera;
	float4x4 camera_to_projection;
	float4   d_lights[2u];
	float4   color_lights[2u];
	float4   color;
}

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
	output.p = mul( input.p, object_to_world);
	output.p = mul(output.p, world_to_camera);
	output.p = mul(output.p, camera_to_projection);
	output.n = mul(float4(input.n, 1.0f), object_to_world).xyz;
	return output;
}