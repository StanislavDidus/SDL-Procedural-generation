struct Input
{
    float4 Color : TEXCOORD0;
};

struct Output
{
    float4 Color : SV_TARGET0;
};

Output main(Input input)
{
    Output output;
    output.Color = input.Color;
    return output;
}