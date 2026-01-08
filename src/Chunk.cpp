#include "Chunk.hpp"

Chunk::Chunk(const SpriteSheet& tileset, const SDL_FRect& rect, int rows, int columns) : rect(rect), tilemap(tileset, rows, columns)
{

}
