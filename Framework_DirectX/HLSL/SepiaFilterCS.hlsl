Texture2D<float4> sourceImage : register(t0);
RWTexture2D<float4> destinationImage : register(u0);
SamplerState statcSampler : register(s0);

[numthreads(8,8,1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint width, height;
    sourceImage.GetDimensions(width, height);
    if (dtid.x < width && dtid.y < height)
    {
        float3x3 toSepia = float3x3(
            0.393, 0.349, 0.272,
            0.769, 0.686, 0.534,
            0.189, 0.168, 0.131);
        float3 color = mul(sourceImage[dtid.xy].xyz, toSepia);
        destinationImage[dtid.xy] = float4(color, 1);
    }
}
