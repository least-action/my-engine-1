cbuffer ConstantBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    float4 vLightDir[2];
    float4 vLightColor[2];
    float4 vOutputColor;
    float4 blackholePos;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float4 Color : COLOR;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = input.Color;
    
    //do NdotL lighting for 2 lights
    for (int i = 0; i < 2; i++)
    {
        finalColor += saturate(dot((float3) vLightDir[i], input.Norm) * vLightColor[i]);
    }
    finalColor.a = 1;
    return finalColor;
}
