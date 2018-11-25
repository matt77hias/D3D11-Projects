cbuffer Camera : register(b0) {
	float4x4 g_world_to_camera;
	float4x4 g_camera_to_projection;
};

cbuffer Model : register(b1) {
	float4x4 g_object_to_world;
	float4   g_color;
};
