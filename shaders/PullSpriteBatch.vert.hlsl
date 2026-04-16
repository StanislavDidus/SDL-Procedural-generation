struct SpriteData
{
    float4 Position_Rotation;
    float4 Size;
    float4 UV;
    float4 Color;
    float4 Flip;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
    uint Flip : TEXCOORD2;
    float4 UV : TEXCOORD3;
};

StructuredBuffer<SpriteData> DataBuffer : register(t0, space0);

cbuffer UniformBlock : register(b0, space1)
{
    float4x4 ViewProjectionMatrix;
};

cbuffer SpriteIndex : register(b1, space1)
{
    uint spriteIndex;
};

static const uint triangleIndices[6] = {0, 1, 2, 2, 3, 0};
static const float2 vertexPos[4] = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f}
};

Output main(uint id : SV_VertexID, uint instance_id : SV_InstanceID)
{
    uint vert = triangleIndices[id % 6];
    SpriteData sprite = DataBuffer[spriteIndex + instance_id];

    float2 texcoord[4] = {
        {sprite.UV.x,               sprite.UV.y              },
        {sprite.UV.x,               sprite.UV.y + sprite.UV.w},
        {sprite.UV.x + sprite.UV.z, sprite.UV.y + sprite.UV.w},
        {sprite.UV.x + sprite.UV.z, sprite.UV.y              }
    };

    float c = cos(sprite.Position_Rotation.w);
    float s = sin(sprite.Position_Rotation.w);

    float2 coord = vertexPos[vert];
    coord *= sprite.Size.xy;

    float3 coordWithDepth = float3(coord + sprite.Position_Rotation.xy, sprite.Position_Rotation.z);
    float3 centrePosition = float3(sprite.Position_Rotation.x + sprite.Size.x * 0.5f, sprite.Position_Rotation.y + sprite.Size.y * 0.5f, sprite.Position_Rotation.z);
    coordWithDepth -= centrePosition;
    float2x2 rotation = {c,s,-s,c};
    coordWithDepth = float3(mul(float2(coordWithDepth.xy), rotation), coordWithDepth.z);
    coordWithDepth += centrePosition;

    Output output;

    output.Position = mul(float4(coordWithDepth, 1.0f), ViewProjectionMatrix);
    output.Texcoord = texcoord[vert];
    output.Color = sprite.Color;
    output.Flip = uint(sprite.Flip.x);
    output.UV = sprite.UV;

    return output;
}
