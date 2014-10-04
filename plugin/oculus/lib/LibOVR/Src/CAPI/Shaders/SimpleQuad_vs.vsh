float2 PositionOffset = float2(0, 0);
float2 Scale = float2(1, 1);

void main(	in  float3 Position		: POSITION,
out float4 oPosition	: SV_Position)
{
	oPosition = float4(Position.xy * Scale + PositionOffset, 0.5, 1.0);
}