cbuffer Model : register(b0) {
	float4x4 object_to_world;
	float4x4 world_to_camera;
	float4x4 camera_to_projection;
	float4   d_lights[2u];
	float4   color_lights[2u];
	float4   color;
}

struct PS_INPUT {
	float4 p_viewport : SV_POSITION;
	float3 n          : NORMAL0;
};

float4 PS(PS_INPUT input) : SV_Target {
	const float3 n = normalize(input.n);
	
	float3 c = 0.0f;
	for (uint i = 0u; i < 2u; ++i) {
		c += dot(d_lights[i].xyz, n) * color_lights[i].xyz;
	}

	return float4(saturate(c), 1.0f);
}