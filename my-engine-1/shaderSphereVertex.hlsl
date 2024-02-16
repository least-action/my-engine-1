#include "Common.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix InvProjection;
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
    float4 Norm : NORMAL;
    float2 Textcoord : TEXCOORD;
};

struct GS_INPUT
{
    float4 PosWorld : SV_POSITION;
    float4 Norm : TEXCOORD0;
    float2 Textcoord : TEXCOORD1;
};

GS_INPUT main(VS_INPUT input)
{
    GS_INPUT output = (GS_INPUT) 0;
    output.PosWorld = mul(input.Pos, World);
    
    output.Norm = mul(input.Norm, World);
    output.Textcoord = input.Textcoord;
    
    return output;
}
