#include "RenderBatch.hpp"

#include "CommandBuffer.hpp"

namespace graphics
{
	graphics::Batch::Batch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline)
		: device{device}
	, graphics_pipeline{graphics_pipeline}
	{
	}

	void graphics::Batch::setMatrix(const glm::mat4& matrix)
	{
		this->matrix = matrix;
	}

	graphics::SpriteBatch::SpriteBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline)
		: Batch{ device, graphics_pipeline } 
	, storage_buffer{device, static_cast<Uint32>(MAX_SPRITES_RENDERED * sizeof(SpriteData)), SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ}
	, transfer_buffer{device, static_cast<Uint32>(MAX_SPRITES_RENDERED * sizeof(SpriteData)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD}
	{

	}

	void graphics::SpriteBatch::addToBatch(const GpuSprite& sprite)
	{
		sprites.push_back(sprite.data);
		this->texture = sprite.texture;
	}

	void graphics::SpriteBatch::flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info)
	{
		if (sprites.empty()) return;

		auto* data = transfer_buffer.map<SpriteData>(first_draw);
		data +=  offset;
		SDL_memcpy(data, sprites.data(), sprites.size() * sizeof(SpriteData));
		transfer_buffer.unmap();
		
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = transfer_buffer.get();
		vertices_transfer_info.offset = offset * sizeof(SpriteData);
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = storage_buffer.get();
		vertices_buffer_region.offset = offset * sizeof(SpriteData);
		vertices_buffer_region.size = sprites.size() * sizeof(SpriteData);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, first_draw);
		SDL_EndGPUCopyPass(copy_pass);
		
		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
		target_info.load_op = SDL_GPU_LOADOP_LOAD;

		SDL_GPUTextureSamplerBinding texture_sampler_binding;
		texture_sampler_binding.texture = texture->get();
		texture_sampler_binding.sampler = texture->getSampler()->get();
		SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);

		SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline->get());
		SDL_BindGPUVertexStorageBuffers(render_pass, 0, &storage_buffer.get(), 1);
		SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &matrix, sizeof(glm::mat4));
		sprite_uniform.index = offset;
		SDL_PushGPUVertexUniformData(command_buffer.get(), 1, &sprite_uniform, sizeof(SpriteUniform));
		SDL_DrawGPUPrimitives(render_pass, 6, sprites.size(), 0, 0);

		SDL_EndGPURenderPass(render_pass);

		offset += sprites.size();

		first_draw = false;
		sprites.clear();
	}

	bool graphics::SpriteBatch::canBatch(const GpuSprite& gpu_sprite) const
	{

		if (sprites.empty())
			return true;

		if (MAX_SPRITES_RENDERED <= sprites.size())
			return false;
		
		return texture == gpu_sprite.texture;
	}

	void graphics::SpriteBatch::reset()
	{
		first_draw = true;
		offset = 0;
		sprites.clear();
	}

	graphics::RectangleBatch::RectangleBatch(std::shared_ptr<SDL_GPUDevice> device,
		std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline)	
		: Batch{device, graphics_pipeline}
	, vertices_buffer{device, static_cast<Uint32>(MAX_RECTANGLES_RENDERED * 6 * sizeof(Vertex)), SDL_GPU_BUFFERUSAGE_VERTEX}
	, transfer_buffer{device, static_cast<Uint32>(MAX_RECTANGLES_RENDERED * 6 * sizeof(Vertex)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD}
	{

	}

	void graphics::RectangleBatch::addToBatch(const RectangleData& rectangle_data)
	{
		vertices.reserve(vertices.size() + 6);

		vertices.emplace_back
		(
			rectangle_data.position.x,
			rectangle_data.position.y,
			0.0f,
			rectangle_data.color.r,
			rectangle_data.color.g,
			rectangle_data.color.b,
			rectangle_data.color.a
		);
		vertices.emplace_back
		(
			rectangle_data.position.x,
			rectangle_data.position.y + rectangle_data.size.y,
			0.0f,
			rectangle_data.color.r,
			rectangle_data.color.g,
			rectangle_data.color.b,
			rectangle_data.color.a
		);
		vertices.emplace_back
		(
			rectangle_data.position.x + rectangle_data.size.x,
			rectangle_data.position.y + rectangle_data.size.y,
			0.0f,
			rectangle_data.color.r,
			rectangle_data.color.g,
			rectangle_data.color.b,
			rectangle_data.color.a
		);
		vertices.emplace_back
		(
			rectangle_data.position.x + rectangle_data.size.x,
			rectangle_data.position.y + rectangle_data.size.y,
			0.0f,
			rectangle_data.color.r,
			rectangle_data.color.g,
			rectangle_data.color.b,
			rectangle_data.color.a
		);

		vertices.emplace_back
		(
			rectangle_data.position.x + rectangle_data.size.x,
			rectangle_data.position.y,
			0.0f,
			rectangle_data.color.r,
			rectangle_data.color.g,
			rectangle_data.color.b,
			rectangle_data.color.a
		);
		vertices.emplace_back
		(
			rectangle_data.position.x,
			rectangle_data.position.y,
			0.0f,
			rectangle_data.color.r,
			rectangle_data.color.g,
			rectangle_data.color.b,
			rectangle_data.color.a
		);
	}

	void graphics::RectangleBatch::flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info)
	{
		if (vertices.empty()) return;

		auto* data = transfer_buffer.map<Vertex>(first_draw);
		data += offset;
		SDL_memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
		transfer_buffer.unmap();
		
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = transfer_buffer.get();
		vertices_transfer_info.offset = offset * sizeof(Vertex);
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = vertices_buffer.get();
		vertices_buffer_region.offset = offset * sizeof(Vertex);
		vertices_buffer_region.size = vertices.size() * sizeof(Vertex);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, first_draw);
		SDL_EndGPUCopyPass(copy_pass);
	
		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
		target_info.load_op = SDL_GPU_LOADOP_LOAD;

		SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline->get());

		SDL_GPUBufferBinding buffer_binding{};
		buffer_binding.offset = offset * sizeof(Vertex);
		buffer_binding.buffer = vertices_buffer.get();
		SDL_BindGPUVertexBuffers(render_pass, 0, &buffer_binding , 1);
		SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &matrix, sizeof(glm::mat4));
		SDL_DrawGPUPrimitives(render_pass, vertices.size(), 1, 0, 0);

		SDL_EndGPURenderPass(render_pass);

		offset += vertices.size();

		first_draw = false;
		vertices.clear();
	}

	bool graphics::RectangleBatch::canBatch(const RectangleData& rectangle_data) const
	{
		return MAX_RECTANGLES_RENDERED * 4 >= vertices.size();
	}

	void graphics::RectangleBatch::reset()
	{
		first_draw = true;
		offset = 0;
		vertices.clear();
	}

	graphics::LineBatch::LineBatch(std::shared_ptr<SDL_GPUDevice> device,
		std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline)
		: Batch{device, graphics_pipeline}
	, line_buffer{device, static_cast<Uint32>(MAX_LINES_RENDERED * 2 * sizeof(Vertex)), SDL_GPU_BUFFERUSAGE_VERTEX}
	, transfer_buffer{device, static_cast<Uint32>(MAX_LINES_RENDERED * 2 * sizeof(Vertex)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD}
	{
	}

	void graphics::LineBatch::addToBatch(const LineData& line_data)
	{
		vertices.emplace_back(line_data.x1, line_data.y1, 0.0f, line_data.color.r, line_data.color.g, line_data.color.b, line_data.color.a);
		vertices.emplace_back(line_data.x2, line_data.y2, 0.0f, line_data.color.r, line_data.color.g, line_data.color.b, line_data.color.a);
	}

	void graphics::LineBatch::flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info)
	{
		if (vertices.empty()) return;
	
		Vertex* data = transfer_buffer.map<Vertex>(first_draw);
		data += offset;
		SDL_memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
		transfer_buffer.unmap();
		
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = transfer_buffer.get();
		vertices_transfer_info.offset = offset * sizeof(Vertex);
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = line_buffer.get();
		vertices_buffer_region.offset = offset * sizeof(Vertex);
		vertices_buffer_region.size = vertices.size() * sizeof(Vertex);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, first_draw);
		SDL_EndGPUCopyPass(copy_pass);
	
		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
		target_info.load_op = SDL_GPU_LOADOP_LOAD;

		SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline->get());

		SDL_GPUBufferBinding buffer_binding{};
		buffer_binding.offset = offset * sizeof(Vertex);
		buffer_binding.buffer = line_buffer.get();
		SDL_BindGPUVertexBuffers(render_pass, 0, &buffer_binding , 1);
		SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &matrix, sizeof(glm::mat4));
		SDL_DrawGPUPrimitives(render_pass, vertices.size(), 1, 0, 0);

		SDL_EndGPURenderPass(render_pass);

		offset += vertices.size();

		first_draw = false;
		vertices.clear();
	}

	bool graphics::LineBatch::canBatch(const LineData& line_data) const
	{
		return MAX_LINES_RENDERED * 2 >= vertices.size();
	}

	void graphics::LineBatch::reset()
	{
		offset = 0;
		vertices.clear();
		first_draw = true;
	}
}