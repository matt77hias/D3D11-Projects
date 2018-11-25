cbuffer Model : register(b0) {
	float4x4 g_object_to_world;
	float4x4 g_world_to_camera;
	float4x4 g_camera_to_projection;
	float4   g_d_lights[2u];
	float4   g_color_lights[2u];
	float4   g_color;
}
