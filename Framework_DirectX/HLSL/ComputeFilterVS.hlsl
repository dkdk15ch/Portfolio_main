struct VSInput
{
    float4 position : POSITION;
    float4 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD;
};

struct SceneParamters
{
    float4x4 proj;
};

ConstantBuffer<SceneParamters> sceneConstants : register(b0);

PSInput main( VSInput In, uint instanceID : SV_InstanceID )
{
    PSInput result = (PSInput) 0;
    
    result.position = mul(In.position, sceneConstants.proj);
    result.uv = In.uv;
	return result;
}