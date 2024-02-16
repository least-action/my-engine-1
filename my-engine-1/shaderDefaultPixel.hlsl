#include "Common.hlsli"

Texture2D _texture : register(t0);
Texture2D _textureDepth : register(t10);
Texture2D _textureDepth2 : register(t11);


SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

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
    float4 Norm : TEXCOORD0;
    float2 Textcoord : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    float3 color = _texture.Sample(MeshTextureSampler, input.Textcoord);
    
    float4 finalColor;
    float distance1 = length(input.PosWorld - Light1.Pos);
    float distance2 = length(input.PosWorld - Light2.Pos);
    
    float3 rlColor = Light1.Intensity / (distance1 * distance1) * dot(input.Norm, normalize(Light1.Pos - input.PosWorld)) * Light1.Color +
        Light2.Intensity / (distance2 * distance2) * dot(input.Norm, normalize(Light2.Pos - input.PosWorld)) * Light2.Color;
    finalColor.rgb = float3(color.r * max(rlColor.r, 0.2f), color.g * max(rlColor.g, 0.2f), color.b * max(rlColor.b, 0.2f));
    finalColor.a = 1;
    
    return finalColor;
}
