#include "Global_b0.hlsli"

struct VSOutput
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 WorldPos : WORLD_POS;
    float4 Color : COLOR0;
};

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

cbuffer LightBuffer : register(b2)
{
    float3 LightPosition : packoffset(c0);
    float3 LightColor : packoffset(c1);
};

cbuffer MaterialBuffer : register(b3)
{
    float3 Diffuse : packoffset(c0);
    float Alpha : packoffset(c0.w);
    float TilingX : packoffset(c1);
    float TilingY : packoffset(c1.y);
};

Texture2D ColorMap      : register(t0);

PSOutput main(VSOutput input)
{    
       
    PSOutput output = (PSOutput) 0; 
    input.TexCoord.x *= TilingX;
    input.TexCoord.y *= TilingY;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(LightPosition - input.WorldPos.xyz);
    
    float4 color = ColorMap.Sample(sampler_lerp, float2(input.TexCoord.x, 1.0 - 
input.TexCoord.y));
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N));
    //diffuse = float3(1.f, 1.f, 1.f);
    output.Color = float4(color.rgb * diffuse, color.a * Alpha);
    output.Color *= input.Color;
      
    //float3 normalColor = N * 0.5f + 0.5f;

    //output.Color = float4(normalColor, 1.0f);
    
    return output;
}