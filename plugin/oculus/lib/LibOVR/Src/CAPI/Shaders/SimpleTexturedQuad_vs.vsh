struct Values
{
   float4 Position : SV_Position;
   float4 Color    : COLOR0;
   float2 TexCoord : TEXCOORD0;
};

float2 PositionOffset = float2(0, 0);
float2 Scale = float2(1, 1);

void main(in float3 Position : POSITION, in float4 Color : COLOR0, in float2 TexCoord : TEXCOORD0, out Values outputValues)
{
   outputValues.Position = float4(Position.xy * Scale + PositionOffset, 0.5, 1.0);
   outputValues.Color    = Color;
   outputValues.TexCoord = TexCoord;
}
