#pragma once

struct Tile
{
	Tile(int index, int raw, int column) : index(index), raw(raw), column(column) {}

	int index;
	int raw;
	int column;
};