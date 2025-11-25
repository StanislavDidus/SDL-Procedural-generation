#pragma once

#include "Noise.hpp"

class ValueNoise : public Noise
{
public:
	ValueNoise();
	~ValueNoise() override;
	
	float noise1D(float x, uint32_t seed) const override;
	float noise2D(float x, float y, uint32_t seed) const override;
};