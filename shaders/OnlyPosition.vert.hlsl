struct Input
{
    float3 Position : TEXCOORD0;
    float4 Color : TEXCOORD1;
};

struct Output
{
    float4 Position : SV_Position;
    float4 Color : TEXCOORD0;
};

cbuffer Screen : register(b0, space1)
{
    int2 size : TEXCOORD0;
};

Output main(Input input)
{
    Output output;
    output.Position.x = input.Position.x / float(size.x) * 2.0f - 1.0f;
    output.Position.y = (input.Position.y / float(size.y) * 2.0f - 1.0f) * -1.0f;
    output.Position.z = input.Position.z;
    output.Position.w = 1.0f;
    output.Color = input.Color;

    return output;
}