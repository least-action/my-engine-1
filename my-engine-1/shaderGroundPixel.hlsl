#include "Common.hlsli"

Texture2D _textureDepth : register(t10);
Texture2D _textureDepth2 : register(t11);

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
    float4 Norm : TEXCOORD0;
    float2 TextCoord : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    
    float4 viewPos = mul(input.PosWorld, transpose(Light1.View));
    float4 viewPro = mul(viewPos, Projection);
    float depth = _textureDepth.Sample(
        DepthSampler,
        float2(
            (viewPro.x / viewPro.w + 1.0) / 2.0,
            (-viewPro.y / viewPro.w + 1.0) / 2.0
        )
    ).r;
    float z = 5.0 * 0.1 / (5.0 - (depth * (5.0 - 0.1)));
    bool isShadow = viewPos.z > z + 0.01;
    
    float4 viewPos2 = mul(input.PosWorld, transpose(Light2.View));
    float4 viewPro2 = mul(viewPos2, Projection);
    float depth2 = _textureDepth2.Sample(
        DepthSampler,
        float2(
            (viewPro2.x / viewPro2.w + 1.0) / 2.0,
            (-viewPro2.y / viewPro2.w + 1.0) / 2.0
        )
    ).r;
    float z2 = 5.0 * 0.1 / (5.0 - (depth2 * (5.0 - 0.1)));
    bool isShadow2 = viewPos2.z > z2 + 0.01;
    
    
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
    float3 rlColor = float3(0.0, 0.0, 0.0);
    
    if (!isShadow)
    {
        float distance1 = length(input.PosWorld - Light1.Pos);
        float3 rl = Light1.Intensity / (distance1 * distance1) * dot(input.Norm, normalize(Light1.Pos - input.PosWorld)) * Light1.Color;
        rlColor.r += rl.r;
        rlColor.g += rl.g;  
        rlColor.b += rl.b;

    }
    if (!isShadow2)
    {
        float distance2 = length(input.PosWorld - Light2.Pos);
        float3 rl2 = Light2.Intensity / (distance2 * distance2) * dot(input.Norm, normalize(Light2.Pos - input.PosWorld)) * Light2.Color;
        rlColor.r += rl2.r;
        rlColor.g += rl2.g;
        rlColor.b += rl2.b;
    }
    
    finalColor.rgb = float3(materialColor.r * max(rlColor.r, 0.2f), materialColor.g * max(rlColor.g, 0.2f), materialColor.b * max(rlColor.b, 0.2f));
    finalColor.a = 1;
    
    return finalColor;
}
