Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
    float2 TexCoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    uint Flip : TEXCOORD2;
};

float4 main(Input input) : SV_TARGET0
{
    if(input.Flip == 1)
    {
        input.TexCoord.x = 1 - input.TexCoord.x;
    }
    else if(input.Flip == 2)
    {
        input.TexCoord.y = 1 - input.TexCoord.y;
    }
    else if(input.Flip == 3)
    {
        input.TexCoord.x = 1 - input.TexCoord.x;
        input.TexCoord.y = 1 - input.TexCoord.y;
    }

    float w;
    float h;
    Texture.GetDimensions(w,h);
    return input.Color * Texture.Sample(Sampler, float2(input.TexCoord.x / w, input.TexCoord.y / h));
}