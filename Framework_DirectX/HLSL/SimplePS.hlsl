struct VSOutput
{
    float4 Position : SV_Position;
    float4 Color    : COLOR;
};

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

float4 main(float4 Position : SV_Position, float4 Color : COLOR) : SV_Target
{
    return Color;
}