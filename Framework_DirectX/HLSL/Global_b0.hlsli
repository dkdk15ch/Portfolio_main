

cbuffer GLOBALE_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_dt;
}

Texture2D NoiseTexture : register(t20); 

SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_lerp_clamp : register(s6);