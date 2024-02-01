#include "Common.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    Light Light1;
    Light Light2;
}

cbuffer WorldBuffer : register(b1)
{
    matrix World;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Textcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float4 Color : COLOR;
    float2 Textcoord : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    
    output.Pos = mul(output.Pos, Projection);
    output.Norm = mul(input.Norm, World);
    output.Color = float4(0.3, 0.5, 0.5, 1.0);
    output.Textcoord = input.Textcoord;
    
    return output;
}
