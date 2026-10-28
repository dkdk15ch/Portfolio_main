struct PSInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD;
};

Texture2D imageTex : register(t0);
SamplerState imageSampler : register(s0);

float4 main(PSInput In) : SV_TARGET
{
    return imageTex.Sample(imageSampler, In.uv.xy);
}