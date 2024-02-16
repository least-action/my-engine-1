#include "Common.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix InvProjection;
    Light Light1;
    Light Light2;
}

cbuffer CenterConstantBuffer : register(b1)
{
    float4 Pos;
}

struct GSInput
{
    float4 PosWorld : SV_POSITION;
    float4 Norm : TEXCOORD0;
    float2 Textcoord : TEXCOORD1;
};

struct GSOutput
{
    float4 Pos : SV_POSITION;
    float4 PosWorld : POSITION0;
    float4 Norm : TEXCOORD0;
    float2 Textcoord : TEXCOORD1;
};

[maxvertexcount(12)]
void main(
	triangle GSInput input[3],
    uint primID : SV_PrimitiveID,
	inout TriangleStream<GSOutput> output
)
{
    GSOutput e0, e1, e2, e01, e12, e20;
    float r = length(input[0].PosWorld.xyz - Pos.xyz);
    e0.PosWorld = input[0].PosWorld;
    e0.Pos = mul(mul(e0.PosWorld, View), Projection);
    e0.Norm = input[0].Norm;
    e0.Textcoord = input[0].Textcoord;
    
    e1.PosWorld = input[1].PosWorld;
    e1.Pos = mul(mul(e1.PosWorld, View), Projection);
    e1.Norm = input[1].Norm;
    e1.Textcoord = input[1].Textcoord;
    
    e2.PosWorld = input[2].PosWorld;
    e2.Pos = mul(mul(e2.PosWorld, View), Projection);
    e2.Norm = input[2].Norm;
    e2.Textcoord = input[2].Textcoord;
    
    e01.PosWorld = Pos + (normalize((e0.PosWorld + e1.PosWorld) / 2.0 - Pos) * r);
    e01.Pos = mul(mul(e01.PosWorld, View), Projection);
    //e01.Pos = (e0.Pos + e1.Pos) / 2.0;
    e01.Norm = normalize((e0.Norm + e1.Norm) / 2.0);
    e01.Textcoord = (e0.Textcoord + e1.Textcoord) / 2.0;
    
    e12.PosWorld = Pos + (normalize((e2.PosWorld + e1.PosWorld) / 2.0 - Pos) * r);
    e12.Pos = mul(mul(e12.PosWorld, View), Projection);
    //e12.Pos = (e2.Pos + e1.Pos) / 2.0;
    e12.Norm = normalize((e2.Norm + e1.Norm) / 2.0);
    e12.Textcoord = (e2.Textcoord + e1.Textcoord) / 2.0;
    
    e20.PosWorld = Pos + (normalize((e0.PosWorld + e2.PosWorld) / 2.0 - Pos) * r);
    e20.Pos = mul(mul(e20.PosWorld, View), Projection);
    //e20.Pos = (e0.Pos + e2.Pos) / 2.0;
    e20.Norm = normalize((e0.Norm + e2.Norm) / 2.0);
    e20.Textcoord = (e0.Textcoord + e2.Textcoord) / 2.0;
    
    // 1
    output.Append(e0);
    output.Append(e20);
    output.Append(e01);
    output.RestartStrip();
    
    
    // 2
    output.Append(e2);
    output.Append(e12);
    output.Append(e20);
    output.RestartStrip();
    
    // 3
    
    output.Append(e1);
    output.Append(e01);
    output.Append(e12);
    output.RestartStrip();
    
    // 4
    
    output.Append(e01);
    output.Append(e20);
    output.Append(e12);
//    output.RestartStrip();
}