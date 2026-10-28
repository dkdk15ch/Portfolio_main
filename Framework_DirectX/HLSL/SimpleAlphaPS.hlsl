#include "Global_b0.hlsli"

struct VSOutput
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR0;
};

struct PSOutput
{
    float4 Color : SV_Target0;
};

cbuffer MaterialBuffer : register(b3)
{
    float3 Diffuse : packoffset(c0);
    float Alpha : packoffset(c0.w);
    float TilingX : packoffset(c1);
    float TilingY : packoffset(c1.y);
};

Texture2D ColorMap : register(t0);

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    input.TexCoord.x *= TilingX;
    input.TexCoord.y *= TilingY;
    
    output.Color = ColorMap.Sample(sampler_lerp, float2(
    input.TexCoord.x,
    1 - input.TexCoord.y));
    
    output.Color *= input.Color;
    //output.Color.a *= Alpha;
    return output;
}