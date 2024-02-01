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
    float4 Norm : NORMAL;
    float2 TextCoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 PosWorld : POSITION0;
    float4 Norm : TEXCOORD0;
    float2 TextCoord : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.PosWorld = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    output.Norm = mul(input.Norm, World);
    output.TextCoord = input.TextCoord;
    
    return output;
}
