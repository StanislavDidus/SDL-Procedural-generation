#pragma once

#include <vector>

template<typename T>
class Grid
{
public:
	Grid(int rows, int columns)
		: rows(rows)
		, columns(columns)
	{
		array.resize(rows * columns);
	}

	int getRows() const
	{
		return rows;
	}

	int getColumns() const
	{
		return columns;
	}

	T& operator()(int x, int y) const
	{
		return array[x + y * columns];
	}

private:
	std::vector<T> array;
	int rows;
	int columns;
};