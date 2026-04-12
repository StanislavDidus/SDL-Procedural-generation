
#include "TileMap.hpp"

#include <utility>

#include "GpuTransferBuffer.hpp"
#include "GpuRenderer.hpp"

graphics::TileMap::TileMap(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuTexture> texture, int world_width_tiles, int world_height_tiles,
                           int tile_width_world, int tile_height_world, int chunk_width_tiles, int chunk_height_tiles)
		: tile_buffer{device, static_cast<Uint32>(world_width_tiles * world_height_tiles * sizeof(TileData)), SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ}
		, sprite_buffer{device, static_cast<Uint32>(world_width_tiles * world_height_tiles * sizeof(Uint32)), SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ}
		, texture{texture}
		, device{device}
		, size{world_width_tiles * world_height_tiles}
{
	GpuTransferBuffer transfer_buffer{ device, static_cast<Uint32>(world_width_tiles * world_height_tiles * sizeof(TileData)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };
	
	TileData* data = transfer_buffer.map<TileData>(false);

	int world_width_chunks = world_width_tiles / chunk_width_tiles;
	int world_height_chunks = world_height_tiles / chunk_height_tiles;
	/*for (int chunk_y = 0; chunk_y < world_height_chunks; ++chunk_y)
	{
		for (int chunk_x = 0; chunk_x < world_width_chunks; ++chunk_x)
		{
			int chunk_x_offset = chunk_x * chunk_width_tiles * tile_width_world;
			int chunk_y_offset = chunk_y * chunk_height_tiles * tile_height_world;

			for (int y = 0; y < chunk_height_tiles; ++y)
			{
				for (int x = 0; x < chunk_width_tiles; ++x)
				{
					data[index++] = TileData
					{
						.position = {chunk_x_offset + x * tile_width_world, chunk_y_offset + y * tile_height_world, 0.0f, 0.0f},
						.size_uv = {static_cast<float>(tile_width_world), static_cast<float>(tile_height_world), 16.0f, 16.0f},
						.color = {1.0f, 1.0f, 1.0f, 1.0f}
					};
				}
			}
		}
	}*/

	for (int y = 0; y < world_height_tiles; ++y)
	{
		for (int x = 0; x < world_width_tiles; ++x)
		{
			int index = x + y * world_width_tiles;
			data[index] = TileData
			{
				.position = {x * tile_width_world, y * tile_height_world, 0.0f, 0.0f},
				.size_uv = {static_cast<float>(tile_width_world), static_cast<float>(tile_height_world), 16.0f, 16.0f},
				.color = {1.0f, 1.0f, 1.0f, 1.0f}
			};
		}
	}

	transfer_buffer.unmap();

	std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };

	if (!command_buffer)
	{
		throw std::runtime_error{ std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()) };
	}

	//std::cout << "Command tile_buffer acquired." << std::endl;

	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

	// Where is
	SDL_GPUTransferBufferLocation location;
	location.transfer_buffer = transfer_buffer.get();
	location.offset = 0;

	// Where to
	SDL_GPUBufferRegion region{};
	region.buffer = tile_buffer.get();
	region.size = world_width_tiles * world_height_tiles * sizeof(TileData);
	region.offset = 0;

	// Upload
	SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

	SDL_EndGPUCopyPass(copy_pass);
}

int graphics::TileMap::getSize() const
{
	return size;
}

void graphics::TileMap::setSpriteMap(const std::vector<Uint32>& sprite_map)
{
	GpuTransferBuffer transfer_buffer{ device, static_cast<Uint32>(sprite_map.size() * sizeof(Uint32)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };
	Uint32* data = transfer_buffer.map<Uint32>(false);
	SDL_memcpy(data, sprite_map.data(), sprite_map.size() * sizeof(Uint32));
	transfer_buffer.unmap();

	std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };

	if (!command_buffer)
	{
		throw std::runtime_error{ std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()) };
	}

	//std::cout << "Command tile_buffer acquired." << std::endl;

	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

	// Where is
	SDL_GPUTransferBufferLocation location;
	location.transfer_buffer = transfer_buffer.get();
	location.offset = 0;

	// Where to
	SDL_GPUBufferRegion region{};
	region.buffer = sprite_buffer.get();
	region.size = sprite_map.size() * sizeof(Uint32);
	region.offset = 0;

	// Upload
	SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

	SDL_EndGPUCopyPass(copy_pass);
}

std::shared_ptr<graphics::GpuTexture> graphics::TileMap::getTexture() const
{
	return texture;
}

SDL_GPUBuffer* graphics::TileMap::getTileBuffer()
{
	return tile_buffer.get();
}

SDL_GPUBuffer* graphics::TileMap::getSpriteBuffer()
{
	return sprite_buffer.get();
}
