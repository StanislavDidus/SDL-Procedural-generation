#pragma once

#include <iostream>
#include <vector>

struct Point
{
	Point() = default;
	Point(float x, float y) : x(x), y(y) {}

	float x;
	float y;

	bool operator< (const Point& other) const { return x < other.x; }
};

class MapRange
{
public:
	MapRange(float inMin, float inMax, float outMin, float outMax);
	~MapRange() = default;

	float getValue(float x) const;
	void addPoint(float x, float y);
private:
	std::vector<Point> points;

	float outMin = 0.f;
	float outMax = 1.f;
};