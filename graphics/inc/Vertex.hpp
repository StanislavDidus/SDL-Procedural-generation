#pragma once

#include <array>
#include <memory>

#include "GpuTexture.hpp"
#include "glm/vec4.hpp"

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
}
