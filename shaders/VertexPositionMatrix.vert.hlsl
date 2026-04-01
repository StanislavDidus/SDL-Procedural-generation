struct Input
{
    float3 Position : TEXCOORD0;
    float4 Color : TEXCOORD1;
};

struct Output
{
    float4 Position : SV_POSITION;
    float4 Color : TEXCOORD0;
};

cbuffer UniformBlock : register(b0, space1)
{
    float4x4 ViewProjectionMatrix;
};

Output main(Input input)
{
    Output output;

    output.Position = mul(float4(input.Position, 1.0f), ViewProjectionMatrix);
    output.Color = input.Color;
    
    return output;
}