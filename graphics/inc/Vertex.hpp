#pragma once

#include <array>
#include <memory>
#include <variant>

#include "TileMap.hpp"
#include "GpuTexture.hpp"
#include "glm/vec4.hpp"
#include "glm/vec2.hpp"

namespace graphics
{
	/*static const char* SamplerNames[] =
	{
		"PointClamp",
		"PointWrap",
		"LinearClamp",
		"LinearWrap",
		"AnisotropicClamp",
		"AnisotropicWrap",
	};*/
	struct Vertex
	{
		float x, y, z;
		float r, g, b, a;
	};
	struct TextureVertex
	{
		float x, y, z;
		float u, v;
	};
	struct TextureObject
	{
		std::string texture_name;
		std::array<TextureVertex, 4> vertices;
	};
	struct RectangleData
	{
		glm::vec2 position;
		glm::vec2 size;
		glm::vec4 color;
	};
	struct LineData
	{
		float x1;
		float y1;
		float x2;
		float y2;
		glm::vec4 color;
	};
	struct TileMapData
	{
		std::shared_ptr<TileMap> tile_map;
	};
	struct alignas(16) SpriteData
	{
		glm::vec4 pos_rot; // Position3 and Rotation1
		glm::vec4 size; //Size2 Padding2
		glm::vec4 uv; // UV4
		glm::vec4 color; // Color4
		glm::vec4 flip; // Flip1 Padding3
	};
	struct GpuSprite
	{
		std::shared_ptr<GpuTexture> texture;
		SpriteData data;
	};

	using DrawData = std::variant<GpuSprite, RectangleData, LineData, TileMapData>;

	struct alignas(16) SpriteUniform
	{
		uint32_t index;
	};

}
