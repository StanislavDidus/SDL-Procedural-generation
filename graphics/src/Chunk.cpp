#include "Chunk.hpp"

#include "CommandBuffer.hpp"
#include "GpuTransferBuffer.hpp"

namespace graphics
{
    Chunk::Chunk(
        std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuTexture> texture,
        int width_tiles, int height_tiles,
        float tile_width_world, float tile_height_world,
        float offset_x, float offset_y,
        int index
        )
    : tile_buffer{device, static_cast<Uint32>(width_tiles * height_tiles * sizeof(TileData)), SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ}
    , sprite_buffer{device, static_cast<Uint32>(width_tiles * height_tiles * sizeof(Uint32)), SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ}
    , rect{offset_x, offset_y, width_tiles * tile_width_world, height_tiles * tile_height_world}
    , device{device}
    , texture{texture}
    , width_tiles{width_tiles}
    , height_tiles{height_tiles}
    , size{width_tiles * height_tiles}
    , index{index}
    {
        GpuTransferBuffer transfer_buffer{ device, static_cast<Uint32>(size * sizeof(TileData)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };
	
        TileData* data = transfer_buffer.map<TileData>(false);
        
        for (int y = 0; y < height_tiles; ++y)
        {
            for (int x = 0; x < width_tiles; ++x)
            {
                int index = x + y * width_tiles;
                data[index] = TileData
                {   
                    .position = {offset_x + x * tile_width_world, offset_y + y * tile_height_world, 0.0f, 1.0f},
                    .size_uv = {static_cast<float>(tile_width_world), static_cast<float>(tile_height_world), 16.0f, 16.0f},
                    .color = {1.0f, 1.0f, 1.0f, 1.0f}
                };
            }
        }

        transfer_buffer.unmap();

        CommandBuffer command_buffer{device};

        SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

        // Where is
        SDL_GPUTransferBufferLocation location;
        location.transfer_buffer = transfer_buffer.get();
        location.offset = 0;

        // Where to
        SDL_GPUBufferRegion region{};
        region.buffer = tile_buffer.get();
        region.size = size * sizeof(TileData);
        region.offset = 0;

        // Upload
        SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

        SDL_EndGPUCopyPass(copy_pass);
    }

    int Chunk::getSize() const
    {
        return size;
    }

    std::shared_ptr<GpuTexture> Chunk::getTexture() const
    {
        return texture;
    }

    void Chunk::setSpriteMap(const std::vector<Uint32>& sprite_map)
    {
        assert(sprite_map.size() > 0 && sprite_map.size() <= size);
        
        GpuTransferBuffer transfer_buffer{ device, static_cast<Uint32>(sprite_map.size() * sizeof(Uint32)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };
        Uint32* data = transfer_buffer.map<Uint32>(false);
        SDL_memcpy(data, sprite_map.data(), sprite_map.size() * sizeof(Uint32));
        transfer_buffer.unmap();

        CommandBuffer command_buffer{device};

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

    const SDL_FRect& Chunk::getRect() const
    {
        return rect;
    }

    int Chunk::getIndex() const
    {
        return index;
    }

    int Chunk::getWidth() const
    {
        return width_tiles;
    }

    int Chunk::getHeight() const
    {
        return height_tiles;
    }

    SDL_GPUBuffer* Chunk::getTileBuffer()
    {
        return tile_buffer.get();
    }

    SDL_GPUBuffer* Chunk::getSpriteBuffer()
    {
        return sprite_buffer.get();
    }
}
