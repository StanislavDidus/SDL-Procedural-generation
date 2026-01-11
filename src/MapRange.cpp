#include "MapRange.hpp"
#include "glm/glm.hpp"
#include <algorithm>

MapRange::MapRange(float inMin, float inMax, float outMin, float outMax) : outMin(outMin), outMax(outMax)
{
	points.emplace_back(inMin, outMin);
	points.emplace_back(inMax, outMax);
}

float MapRange::getOutMin() const
{
	return outMin;
}

float MapRange::getOutMax() const
{
	return outMax;
}

float MapRange::getValue(float x) const
{
	auto it = std::lower_bound(points.begin(), points.end(), x, 
		[](const Point& p, float value) {
			return p.x < value;
		});

	if (it == points.begin())
	{
		return outMin;
	}
	else if (it == points.end())
	{
		return outMax;
	}
	else
	{
		auto left = *(it - 1);
		auto right = *it;

		float t = (x - left.x) / (right.x - left.x);

		float result = glm::mix(left.y, right.y, t);

		return result;
	}
}

void MapRange::addPoint(float x, float y)
{
	points.emplace_back(x, y);

	std::sort(points.begin(), points.end(), 
		[](const Point& a, const Point& b) {
			return a.x < b.x; 
		});
}