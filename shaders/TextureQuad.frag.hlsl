Texture2D<float4> Texture : register(t0, space0);
SamplerState Sampler : register(s0, space0);

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{
    return Texture.Sample(Sampler, TexCoord);
}
