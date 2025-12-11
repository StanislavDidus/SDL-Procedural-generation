#include "PerlynNoise.hpp"
#include "glm/vec2.hpp"

//Takes the value in a range of 0 to 1
//Returns a value in a range of -1 to 1
inline float getRangeMinusOneToOne(float value)
{
	return value * 2.f - 1.f;
}

inline glm::vec2 RandGradFloat(float value, uint32_t seed)
{
	float theta = acos(2.f * getHashValue(value, seed) - 1);
	float phi = getHashValue(value + 2, seed) * 6.28318530718f;

	float x = cos(phi) * sin(theta);
	float y = sin(phi) * sin(theta);

	return glm::vec2{x, y};
}

constexpr float fifthOrderInterpolate(float x)
{
	return x * x * x * (6.f * x * x - 15 * x + 10);
}

inline float MinusOneOrOne(uint32_t v, uint32_t seed)
{
	float r = getHashValue(v, seed);
	return r < 0.5f ? -1.f : 1.f;
}

float PerlynNoise::noise1D(float x, uint32_t seed) 
{
	int floor_x = static_cast<int>(std::floor(x));
	int ceil_x = floor_x + 1;

	/*float gradient[2]
	{
		getRangeMinusOneToOne(getHashValue(floor_x, seed)),
		getRangeMinusOneToOne(getHashValue(ceil_x, seed))
	};*/

	float gradient[2]
	{
		MinusOneOrOne(floor_x, seed),
		MinusOneOrOne(ceil_x, seed)
	};

	float distance[2]
	{
		x - floor_x,
		x - ceil_x
	};

	float tx = smoothStep(distance[0]);

	float noise_value = glm::mix(gradient[0] * distance[0], gradient[1] * distance[1], tx);

	return noise_value * 0.5f + 0.5f;
}

float PerlynNoise::noise2D(float x, float y, uint32_t seed) 
{
	int floor_x = static_cast<int>(std::floor(x));
	int ceil_x = floor_x + 1;

	int floor_y = static_cast<int>(std::floor(y));
	int ceil_y = floor_y + 1;

	glm::vec2 gradient[4]
	{
		glm::normalize(glm::vec2{getRangeMinusOneToOne(getHashValue(floor_x, floor_y, seed)),
			  getRangeMinusOneToOne(getHashValue(floor_x, floor_y, seed + 1))}),
		glm::normalize(glm::vec2{getRangeMinusOneToOne(getHashValue(ceil_x, floor_y, seed)),
				  getRangeMinusOneToOne(getHashValue(ceil_x, floor_y, seed + 1))}),
		glm::normalize(glm::vec2{getRangeMinusOneToOne(getHashValue(ceil_x, ceil_y, seed)),
				  getRangeMinusOneToOne(getHashValue(ceil_x, ceil_y, seed + 1))}),
		glm::normalize(glm::vec2{getRangeMinusOneToOne(getHashValue(floor_x, ceil_y, seed)),
				  getRangeMinusOneToOne(getHashValue(floor_x, ceil_y, seed + 1))})
	};

	/*glm::vec2 gradient[4]
	{
		RandGradFloat(getWholeHash(floor_x, floor_y, seed), seed),
		RandGradFloat(getWholeHash(ceil_x, floor_y, seed), seed),
		RandGradFloat(getWholeHash(ceil_x, ceil_y, seed), seed),
		RandGradFloat(getWholeHash(floor_x, ceil_y, seed), seed),
	};*/

	glm::vec2 point{ x, y };

	glm::vec2 distance[4]
	{
		point - glm::vec2{floor_x, floor_y},
		point - glm::vec2{ceil_x, floor_y},
		point - glm::vec2{ceil_x, ceil_y},
		point - glm::vec2{floor_x, ceil_y}
	};

	float tx = fifthOrderInterpolate(x - floor_x);
	float ty = fifthOrderInterpolate(y - floor_y);

	float noise_value = glm::mix(glm::mix(glm::dot(gradient[0], distance[0]), glm::dot(gradient[1], distance[1]), tx), glm::mix(glm::dot(gradient[3], distance[3]), glm::dot(gradient[2], distance[2]), tx), ty);

	return noise_value * 0.5f + 0.5f;
}
