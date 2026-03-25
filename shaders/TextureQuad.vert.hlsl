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

    output.Position.x = input.Position.x / float(960) * 2.0f - 1.0f;
    output.Position.y = (input.Position.y / float(540) * 2.0f - 1.0f) * -1.0f;
    output.Position.z = input.Position.z;
    output.Position.w = 1.0f;

    return output;
}