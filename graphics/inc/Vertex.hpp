#pragma once

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
}
