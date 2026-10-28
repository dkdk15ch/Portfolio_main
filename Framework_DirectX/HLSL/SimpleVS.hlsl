struct VSInput
{
    float3 Position : POSITION;    //  位置情報(x,y,z)
    float4 Color : COLOR;           //  頂点カラー
};

struct VSOutput
{
    float4 Position : SV_Position;  //  位置座標(x,y,z,w)
    float4 Color : COLOR;
};

cbuffer Transform : register(b1)
{
    float4x4 World : packoffset(c0);
    float4x4 View : packoffset(c4);
    float4x4 Proj : packoffset(c8);
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;     //  cast
    
    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos  = mul(View, worldPos);
    float4 projPos  = mul(Proj, viewPos);

    output.Position = projPos;
    output.Color = input.Color;
    
	return output;
}