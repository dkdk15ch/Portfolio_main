Texture2D<float4> sourceImage : register(t0);
RWTexture2D<float4> destinationImage : register(u0);
SamplerState statcSampler : register(s0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width, height;
    sourceImage.GetDimensions(width, height);
    if (DTid.x < width && DTid.y < height)
    {
        float4 rawColor = sourceImage[DTid.xy];
        if (rawColor.a == 0.0)
        {
            destinationImage[DTid.xy] = float4(0, 0, 0, 1);
            return;
        }
        
        // 輝度の計算
        float luma = dot(rawColor.rgb, float3(0.2126, 0.7152, 0.0722));
        
        // 2. 閾値の判定
        float threshold = 0.8;
        
        float3 extractedColor = float3(0, 0, 0);
        if (luma > threshold)
        {
            // 閾値を超えていたら、元の色
            extractedColor = rawColor.rgb;
        }

        destinationImage[DTid.xy] = float4(extractedColor, 1.0);
    }
}