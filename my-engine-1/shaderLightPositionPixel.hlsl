#include "Common.hlsli"

Texture2D _texture : register(t0);
Texture2D _textureDepth : register(t10);
Texture2D _textureDepth2 : register(t11);

SamplerState DepthSampler : register(s0);

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
    float4 Pos2 : POSITION1;
    float4 Norm : TEXCOORD0;
    float2 Textcoord : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(1.0, 1.0, 1.0, 1.0);
}
