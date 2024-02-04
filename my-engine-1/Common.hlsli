struct Light
{
    float4 Pos;
    float4 Color;
    float Intensity;
    float3 _buffer0;
    
    uint Type;
    float3 _buffer1;
    
    matrix View;
};


struct SharedConstantBuffer
{
    matrix View;
    matrix Projection;
    Light Light1;
    Light Light2;
};
