#pragma once

#include "Noise.hpp"

class PerlynNoise
{
public:
	PerlynNoise() = default;
	~PerlynNoise() = default;

	static float noise1D(float x, uint32_t seed);
	static float noise2D(float x, float y, uint32_t seed);
};