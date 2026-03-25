#pragma once

#include <array>

namespace graphics
{
	static const char* SamplerNames[] =
	{
		"PointClamp",
		"PointWrap",
		"LinearClamp",
		"LinearWrap",
		"AnisotropicClamp",
		"AnisotropicWrap",
	};
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
}
