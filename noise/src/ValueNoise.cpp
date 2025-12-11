#include "ValueNoise.hpp"

float ValueNoise::noise1D(float x, uint32_t seed)
{
	int floor_x = static_cast<int>(std::floor(x));
	int ceil_x = floor_x + 1;

	float arr[2]{
		getHashValue(floor_x, seed),
		getHashValue(ceil_x, seed)
	};

	float tx = smoothStep(x - floor_x);

	float noise_value = glm::mix(arr[0], arr[1], tx);

	return noise_value;
}

float ValueNoise::noise2D(float x, float y, uint32_t seed)
{
	int floor_x = static_cast<int>(std::floor(x));
	int ceil_x = floor_x + 1;

	int floor_y = static_cast<int>(std::floor(y));
	int ceil_y = floor_y + 1;

	float arr[4]{
		getHashValue(floor_x, floor_y, seed),
		getHashValue(ceil_x, floor_y, seed),
		getHashValue(ceil_x, ceil_y, seed),
		getHashValue(floor_x, ceil_y, seed),
	};

	float tx = smoothStep(x - floor_x);
	float ty = smoothStep(y - floor_y);

	float noise_value = glm::mix(glm::mix(arr[0], arr[1], tx), glm::mix(arr[3], arr[2], tx), ty);

	return noise_value;
}
