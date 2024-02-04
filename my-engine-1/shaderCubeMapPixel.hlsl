#include "Common.hlsli"

Texture2D _textureXP : register(t0);
Texture2D _textureXN : register(t1);
Texture2D _textureZP : register(t2);
Texture2D _textureZN : register(t3);
Texture2D _textureYP : register(t4);
Texture2D _textureYN : register(t5);


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
    float4 OriginPos : COLOR;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = float4(1.0, 0.0, 0.0, 1.0);
    float4 np = normalize(input.OriginPos);
    if (abs(np.z) > abs(np.x) && abs(np.z) > abs(np.y))
    {
        if (np.z > 0.0)
            color = _textureZP.Sample(MeshTextureSampler, float2(((np.x / np.z) + 1.0) / 2.0, ((-np.y / np.z) + 1.0) / 2.0));
        else
            color = _textureZN.Sample(MeshTextureSampler, float2(((np.x / np.z) + 1.0) / 2.0, ((np.y / np.z) + 1.0) / 2.0));
    }
    else if (abs(np.x) > abs(np.z) && abs(np.x) > abs(np.y))
    {
        if (np.x > 0.0)
            color = _textureXP.Sample(MeshTextureSampler, float2(((-np.z / np.x) + 1.0) / 2.0, ((-np.y / np.x) + 1.0) / 2.0));
        else
            color = _textureXN.Sample(MeshTextureSampler, float2(((-np.z / np.x) + 1.0) / 2.0, ((np.y / np.x) + 1.0) / 2.0));
    }
    else
    {
        if (np.y > 0.0)
            color = _textureYP.Sample(MeshTextureSampler, float2(((np.x / np.y) + 1.0) / 2.0, ((np.z / np.y) + 1.0) / 2.0));
        else
            color = _textureYN.Sample(MeshTextureSampler, float2(((-np.x / np.y) + 1.0) / 2.0, ((np.z / np.y) + 1.0) / 2.0));

    }
    
    return color;
}
