cbuffer UniformBlock : register(b0, space1)
{
    float4x4 MatrixTransform;
};

struct Input
{
    float3 Position : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
};

struct Output
{
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_POSITION;
};

Output main(Input input)
{
    Output output;
    output.TexCoord = input.TexCoord;
    output.Position = mul(float4(input.Position.x, input.Position.y, input.Position.z, 1.0f), MatrixTransform);

    return output;
}