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

	void reset()
	{
		array.clear();
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

	template<typename Self>
	auto&& data(this Self&& self);
	
	const T& operator()(int x, int y) const
	{
		return array[x + y * columns];
	}

	T& operator()(int x, int y)
	{
		return array[x + y * columns];
	}

private:
	std::vector<T> array;
	int rows;
	int columns;
};

template <typename T>
template <typename Self>
auto&& Grid<T>::data(this Self&& self)
{
	return self.array;
}
