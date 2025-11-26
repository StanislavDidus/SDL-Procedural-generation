#pragma once

struct Tile
{
	Tile(int index, int row, int column) : index(index), row(row), column(column) {}

	int index;
	int row;
	int column;
};