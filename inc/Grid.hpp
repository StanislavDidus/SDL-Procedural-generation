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
	
	template<typename Self>
	auto&& operator()(this Self&& self, int x, int y);

	template<typename Self>
	auto&& operator()(this Self&& self, int index);

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

template <typename T>
template <typename Self>
auto&& Grid<T>::operator()(this Self&& self, int x, int y)
{
	return self.array[x + y * self.columns];
}

template <typename T>
template <typename Self>
auto&& Grid<T>::operator()(this Self&& self, int index)
{
	return data[index];
}
