#pragma once

#include <cstdint>

#include "SDL3/SDL_pixels.h"

namespace graphics
{

	struct Color
	{
		Color() = default;
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
		Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
		Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
		~Color() = default;

		Color operator+ (const Color& operand) const
		{
			return { static_cast<uint8_t>(r + operand.r),
					static_cast<uint8_t>(g + operand.g),
					static_cast<uint8_t>(b + operand.b),
					static_cast<uint8_t>(a + operand.a) };
		}
		Color operator- (const Color& operand) const
		{
			return { static_cast<uint8_t>(r - operand.r),
					static_cast<uint8_t>(g - operand.g),
					static_cast<uint8_t>(b - operand.b),
					static_cast<uint8_t>(a - operand.a)
			};
		}
		Color operator/ (float operand) const
		{
			return { static_cast<uint8_t>(r / operand),
					static_cast<uint8_t>(g / operand),
					static_cast<uint8_t>(b / operand),
					a
			};
		}

		explicit operator SDL_Color() const
		{
			return SDL_Color{ r, g, b, a };
		}

		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 255;

		static const Color BLACK;
		static const Color GREY;
		static const Color WHITE;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color YELLOW;
		static const Color TRANSPARENT_BLUE;
		static const Color TRANSPARENT_RED;
		static const Color TRANSPARENT_GREEN;
	};

	static Color mix(const Color& c1, const Color& c2)
	{
		return (c1 + c2) / 2.f;
	}

} // namespace graphics