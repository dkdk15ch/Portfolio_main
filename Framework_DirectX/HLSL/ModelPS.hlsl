#include "Global_b0.hlsli"
struct VSOutput
{    
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float3 Tangent : TANGENT;
    float2 Texcoord1 : TEXCOORD0;
    float2 Texcoord2 : TEXCOORD1;
    float4 WorldPos : WORLD_POS;

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
    float4 Specular : packoffset(c1);
};

Texture2D ColorMap : register(t0);

PSOutput main(VSOutput In)
{
    PSOutput output = (PSOutput) 0;
    
    float3 N = normalize(In.Normal);
    float3 L = normalize(LightPosition - In.WorldPos.xyz);
    float4 color = ColorMap.Sample(sampler_lerp, float2(In.Texcoord1.x, 
In.Texcoord1.y));
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N));
    
    output.Color = float4(color.rgb * diffuse * In.Color.xyz, color.a * Alpha);
    //output.Color = In.Color;
    
    return output;
}