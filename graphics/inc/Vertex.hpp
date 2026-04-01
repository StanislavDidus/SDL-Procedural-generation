#pragma once

#include <array>
#include <memory>

#include "GpuTexture.hpp"

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
	struct SpriteData
	{
		float x, y, z;
		float rotation;
		float w, h, padding_a = 0.0f, padding_b = 0.0f;
		float tex_u, tex_v, tex_w, tex_h;
		float r, g, b, a;
		unsigned int flip;
	};
	struct GpuSprite
	{
		std::shared_ptr<GpuTexture> texture;
		SpriteData data;
	};
}
