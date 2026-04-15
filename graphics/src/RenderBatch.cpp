#include "RenderBatch.hpp"

#include "CommandBuffer.hpp"

graphics::SpriteBatch::SpriteBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline)
	: device{device}
	, graphics_pipeline{graphics_pipeline}
	, storage_buffer{device, static_cast<Uint32>(1000 * sizeof(SpriteData)), SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ}
	, transfer_buffer{device, static_cast<Uint32>(1000 * sizeof(SpriteData)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD}
{

}

void graphics::SpriteBatch::setMatrix(const glm::mat4& matrix)
{
	world_matrix = matrix;
}

void graphics::SpriteBatch::addToBatch(const SpriteData& sprite_data, std::shared_ptr<GpuTexture> texture)
{
	sprites.push_back(sprite_data);
	this->texture = texture;
}

void graphics::SpriteBatch::flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info)
{
	if (sprites.empty()) return;

	auto* data = transfer_buffer.map<SpriteData>(first_draw);
	data += sprite_offset;
	SDL_memcpy(data, sprites.data(), sprites.size() * sizeof(SpriteData));
	transfer_buffer.unmap();
		
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

	// Upload vertices
	SDL_GPUTransferBufferLocation vertices_transfer_info = {};
	vertices_transfer_info.transfer_buffer = transfer_buffer.get();
	vertices_transfer_info.offset = sprite_offset * sizeof(SpriteData);
	SDL_GPUBufferRegion vertices_buffer_region = {};
	vertices_buffer_region.buffer = storage_buffer.get();
	vertices_buffer_region.offset = sprite_offset * sizeof(SpriteData);
	vertices_buffer_region.size = sprites.size() * sizeof(SpriteData);

	SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, first_draw);
	SDL_EndGPUCopyPass(copy_pass);

	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
	if (first_draw) target_info.load_op = SDL_GPU_LOADOP_LOAD;

	SDL_GPUTextureSamplerBinding texture_sampler_binding;
	texture_sampler_binding.texture = texture->get();
	texture_sampler_binding.sampler = texture->getSampler()->get();
	SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);

	SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline->get());
	SDL_BindGPUVertexStorageBuffers(render_pass, 0, &storage_buffer.get(), 1);
	SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &world_matrix, sizeof(glm::mat4));
	for (int i = 0; i < sprites.size(); ++i)
	{
		sprite_uniform.index = i + sprite_offset;

		SDL_PushGPUVertexUniformData(command_buffer.get(), 1, &sprite_uniform, sizeof(SpriteUniform));
		SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
	}

	SDL_EndGPURenderPass(render_pass);

	sprite_offset += sprites.size();

	first_draw = false;
	sprites.clear();
}

bool graphics::SpriteBatch::canBatch(const DrawData& draw_data) const
{
	if (draw_data.type != DrawData::Type::Sprite)
	{
		return false;
	}

	if (sprites.empty())
		return true;

	return texture == draw_data.texture;
}

void graphics::SpriteBatch::reset()
{
	first_draw = true;
	sprite_offset = 0;
	sprites.clear();
}
