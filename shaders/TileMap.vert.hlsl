struct TileData
{
    float4 Position;
    float4 Size_UV;
    float4 Color;
};

struct Output
{
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Position : SV_Position;
    uint Flip : TEXCOORD2;
    float4 UV : TEXCOORD3;
};

StructuredBuffer<TileData> DataBuffer : register(t0, space0);
StructuredBuffer<uint> SpriteMap : register(t1, space0);

cbuffer Projection : register(b0, space1)
{
    float4x4 ViewProjectionMatrix;
}

static const uint triangleIndices[6] = {0, 1, 2, 2, 3, 0};
static const float2 vertexPos[4] = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f}
};

Output main(uint id : SV_VertexID, uint instance_id : SV_InstanceID)
{
    uint vert = triangleIndices[id];
    TileData tile = DataBuffer[instance_id];
    uint sprite_id = SpriteMap[instance_id];

    float2 texcoord[4] = {
        {tile.Size_UV.z * sprite_id + 1.0f,                 0.0f                                     },
        {tile.Size_UV.z * sprite_id + 1.0f,                 tile.Size_UV.z                           },
        {tile.Size_UV.z * (sprite_id + 1) - 1.0f,           tile.Size_UV.z                           },
        {tile.Size_UV.z * (sprite_id + 1) - 1.0f,           0.0f                                     }
    };

    float2 coord = vertexPos[vert];
    coord *= tile.Size_UV.xy;
    float3 coordWithDepth = float3(coord + tile.Position.xy, 0.0f);

    Output output;

    output.Position = mul(float4(coordWithDepth, 1.0f), ViewProjectionMatrix);
    output.Texcoord = texcoord[vert];
    output.Color = tile.Color;
    output.Flip = 0;
    output.UV = float4(0.0f, 0.0f, tile.Size_UV.x, tile.Size_UV.y);

    return output;
}