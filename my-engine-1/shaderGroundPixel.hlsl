#include "Common.hlsli"

Texture2D _textureDepth : register(t10);

SamplerState DepthSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix InvProjection;
    Light Light1;
    Light Light2;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 PosWorld : POSITION0;
    float4 Pos2 : POSITION1;
    float4 Norm : TEXCOORD0;
    float2 TextCoord : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    
    //float4 dd = mul(input.Pos2, InvProjection);
    float depth = _textureDepth.Sample(DepthSampler, float2((input.Pos2.x / input.Pos2.w + 1.0) / 2.0, (-input.Pos2.y / input.Pos2.w + 1.0) / 2.0)).r;
    float z = 5.0 * 0.01 / (5.0 - (depth * (5.0 - 0.01)));
    //float z = (depth - 0.96) / 0.04;
    
    float4 cc;
    cc.r = z;
    cc.g = 0.0;
    cc.b = 0.0;
    cc.a = 1.0;
    return cc;
    
    float4 aColor = { 0.3, 0.3, 0.3, 1.0 };
    float4 bColor = { 0.5, 0.5, 0.5, 1.0 };
    float3 materialColor;
    int devide = 20;
    int xth = fmod(floor(input.TextCoord.x * devide), 2);
    int yth = fmod(floor(input.TextCoord.y * devide), 2);
    if (xth + yth == 0 || xth + yth == 2)
        materialColor = aColor;
    else
        materialColor = bColor;
    
    float4 finalColor;
    float distance1 = length(input.PosWorld - Light1.Pos);
    float distance2 = length(input.PosWorld - Light2.Pos);
    float receiveLight = Light1.Intensity / (distance1 * distance1) * dot(input.Norm, normalize(Light1.Pos - input.PosWorld)) +
        Light2.Intensity / (distance2 * distance2) * dot(input.Norm, normalize(Light2.Pos - input.PosWorld));
    finalColor.rgb = materialColor * max(receiveLight, 0.2f);
    finalColor.a = 1;
    
    
    return finalColor;
}
