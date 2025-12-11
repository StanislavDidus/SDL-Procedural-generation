#include "Biomes.hpp"
#include <algorithm>

float Biome::weight(float temperature_, float moisture_) const
{
	float distance = std::sqrt(std::pow((temperature - temperature_), 2) + std::pow((moisture - moisture_), 2));

	float d = distance / 1.1f;
	d = std::min(d, 1.f);
	float w = 1.f - d;
	return w;
	
}
