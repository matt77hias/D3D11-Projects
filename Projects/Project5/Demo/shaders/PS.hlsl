struct PS_INPUT {
	float4 p_viewport : SV_POSITION;
	float4 color      : COLOR0;
};

float4 PS(PS_INPUT input) : SV_Target {
	return input.color;
}