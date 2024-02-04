#include "Common.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    matrix _;
    matrix Projection;
    matrix InvProjection;
    Light Light1;
    Light Light2;
}

cbuffer ViewBuffer : register(b1)
{
    matrix View;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 OriginPos : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.OriginPos = input.Pos;
    output.Pos = mul(input.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}
