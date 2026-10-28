struct VSInput
{
    float3 Position : POSITION;    //  
    float3 Normal   : NORMAL;       //  
    float2 TexCoord : TEXCOORD;     //  テクスチャ座標
    float3 Tangent  : TANGENT;      //  
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
    float4 WorldPos : WORLD_POS;
    float4 Color    : COLOR0;
};

struct InstanceData
{
    float4x4 World; // ワールド行列です.
    float4 Color; //  カラー
};

cbuffer Transform : register(b1)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
};

cbuffer InstatnceParameter : register(b4)
{
    InstanceData Data[100];
};

VSOutput main(VSInput input,uint instanceID : SV_InstanceID)
{
    VSOutput output = (VSOutput) 0;
    
    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(Data[instanceID].World, localPos);
    float4 viewPos  = mul(View, worldPos);
    float4 projPos  = mul(Proj, viewPos);
    
    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.WorldPos = worldPos;
    output.Normal = normalize(mul((float3x3) Data[instanceID].World, input.Normal));
    output.Color = Data[instanceID].Color;
    
    return output;
}