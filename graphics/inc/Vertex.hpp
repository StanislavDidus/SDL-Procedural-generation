#pragma once

#include <array>

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
		float w, h, padding_a, padding_b;
		float tex_u, tex_v, tex_w, tex_h;
		float r, g, b, a;
	};
}
