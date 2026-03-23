struct Input
{
    float4 Color : TEXCOORD0;
};

struct Output
{
    float4 Color : SV_TARGET0;
};

cbuffer Uniforms : register(b0, space3)
{
    float time;
}

Output main(Input input)
{
    Output output;
    float pulse = sin(time * 2.0f) * 0.5f + 0.5f;
    output.Color = float4(input.Color.rgb * (0.8f + pulse * 0.5f), input.Color.a);
    return output;
}