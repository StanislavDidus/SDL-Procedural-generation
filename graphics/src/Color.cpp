#include "Color.hpp"

namespace graphics
{
	const Color Color::BLACK(0, 0, 0);
	const Color Color::GREY(128, 128, 128);
	const Color Color::WHITE(255, 255, 255);
	const Color Color::RED{ 255,0,0 };
	const Color Color::GREEN{ 0,255,0 };
	const Color Color::BLUE{ 0,0,255 };
	const Color Color::YELLOW(Color::RED + Color::GREEN);
	const Color Color::TRANSPARENT_BLUE{ 0, 0, 255, 50 };
	const Color Color::TRANSPARENT_RED{ 255, 0, 0, 50 };
	const Color Color::TRANSPARENT_GREEN{ 0,255,0, 50 };
} // namespace graphics