#include "Chunk.hpp"

using namespace graphics;

Chunk::Chunk(const SpriteSheet& tileset, const SDL_FRect& rect, int rows, int columns) : rect(rect), tilemap(rows, columns)
{

}
