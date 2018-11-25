cbuffer Camera : register(b0) {
	float4x4 world_to_camera;
	float4x4 camera_to_projection;
};

cbuffer Model : register(b1) {
	float4x4 object_to_world;
	float4   color;
};
