cbuffer Model : register(b0) {
	float4x4 g_object_to_world      : packoffset(c0);
	float4x4 g_world_to_camera      : packoffset(c4);
	float4x4 g_camera_to_projection : packoffset(c8);
	float4   g_d_lights[2u]         : packoffset(c12);
	float4   g_color_lights[2u]     : packoffset(c14);
	float4   g_color                : packoffset(c16);
}
