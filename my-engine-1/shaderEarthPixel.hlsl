#include "Common.hlsli"

Texture2D _texture : register(t0);
//SamplerState _sampler : register(s0);

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
    Light Light1;
    Light Light2;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float4 Color : COLOR;
    float2 Textcoord : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return _texture.Sample(MeshTextureSampler, input.Textcoord);
}
