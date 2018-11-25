cbuffer Camera : register(b0) {
	float4x4 g_world_to_camera      : packoffset(c0);
	float4x4 g_camera_to_projection : packoffset(c4);
};

cbuffer Model : register(b1) {
	float4x4 g_object_to_world      : packoffset(c0);
	float4   g_color                : packoffset(c4);
};
