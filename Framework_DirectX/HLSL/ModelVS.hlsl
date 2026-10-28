struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR;
    float2 Texcoord1 : TEXCOORD0;
    float2 Texcoord2 : TEXCOORD1;
    uint4 BoneId : BLENDINDICES;
    float4 BoneWeight : BLENDWEIGHT;
};

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



cbuffer Transform : register(b1)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
};

struct InstanceData
{
    float4x4 World; // ワールド行列です.
    float4 Color; //  カラー
};

cbuffer InstatnceParameter : register(b4)
{
    InstanceData Data[100];
};

cbuffer BoneParameter : register(b5)
{
    float4x4 boneMatrices[256];
};

float4 TransformPosition(float4 inPos, VSInput In)
{
    float4 pos = 0;
    uint indices[4] = (uint[4])In.BoneId;
    float weight[4] = (float[4]) In.BoneWeight;
    
    float totalWeight = In.BoneWeight.x + In.BoneWeight.y + In.BoneWeight.z + In.BoneWeight.w;
    
    for (int i = 0; i < 4; ++i)
    {
        if (weight[i] <= 0.0001f || indices[i] >= 256)
        {
            continue;
        }
        
        
        float4x4 mtx = boneMatrices[indices[i]];
        float w = weight[i];
        pos += mul(inPos, mtx) * w;
    }
    pos.w = 1;
    return pos;
}

float3 TransformNormal(float3 inNormal, VSInput In)
{
    float3 normal = 0;
    uint indices[4] = (uint[4]) In.BoneId;
    float weight[4] = (float[4]) In.BoneWeight;
    for (int i = 0; i < 4; ++i)
    {
        if (weight[i] <= 0.0001f || indices[i] >= 256)
        {
            continue;
        }
        float4x4 mtx = boneMatrices[indices[i]];
        float w = weight[i];
        normal += mul(inNormal, (float3x3) mtx) * w;
        
    }
    return normal;
}

VSOutput main(VSInput In, uint instanceID : SV_InstanceID)
{
    VSOutput result = (VSOutput) 0;
        
        float4 inPos;
        inPos.xyz = In.Position;
        inPos.w = 1.0f;
        float4 pos = TransformPosition(inPos, In);
        float4 worldPos = mul(Data[instanceID].World, pos);
        float4 viewPos = mul(View, worldPos);
        float4 projPos = mul(Proj, viewPos);
    
        float3 normal = TransformNormal(In.Normal, In);
        float3 worldNormal = mul((float3x3) Data[instanceID].World, normal);
    
        result.Position = projPos;
        result.Normal = normalize(worldNormal);
        result.Color = Data[instanceID].Color;
        result.Texcoord1 = In.Texcoord1;
        result.Texcoord2 = In.Texcoord2;
        result.WorldPos = worldPos;
    

    return result;
}