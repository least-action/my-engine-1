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
    float2 TextCoord : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    float4 aColor = { 0.3, 0.3, 0.1, 1.0 };
    float4 bColor = { 0.1, 0.3, 0.3, 1.0 };
    float4 finalColor;
    int devide = 20;
    int xth = fmod(floor(input.TextCoord.x * devide), 2);
    int yth = fmod(floor(input.TextCoord.y * devide), 2);
    if (xth + yth == 0 || xth + yth == 2)
        finalColor = aColor;
    else
        finalColor = bColor;
    
    
    //do NdotL lighting for 2 lights
    for (int i = 0; i < 2; i++) {
        finalColor += saturate(dot((float3) vLightDir[i], input.Norm) * vLightColor[i]);
    }
    finalColor.a = 1;
    return finalColor;
}
