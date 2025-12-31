#pragma once

#include <cstdint>

struct Color
{
	Color() = default;
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
	Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
	Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
	~Color() {}

	Color operator+ (const Color& opperand) const
	{
		return { static_cast<uint8_t>(r + opperand.r),
				static_cast<uint8_t>(g + opperand.g),
				static_cast<uint8_t>(b + opperand.b),
				static_cast<uint8_t>(a + opperand.a) };
	}
	Color operator- (const Color& opperand) const
	{
		return { static_cast<uint8_t>(r - opperand.r),
				static_cast<uint8_t>(g - opperand.g),
				static_cast<uint8_t>(b - opperand.b),
				static_cast<uint8_t>(a - opperand.a)
		};
	}

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	static const Color BLACK;
	static const Color GREY;
	static const Color WHITE;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
};