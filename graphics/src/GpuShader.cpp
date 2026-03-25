#include "GpuShader.hpp"

graphics::GpuShader::GpuShader(std::shared_ptr<SDL_GPUDevice> device, const std::filesystem::path& path, int sampler_count, int uniform_count)
	: device{device}
{
	SDL_GPUShaderStage stage;

	if (SDL_strstr(path.filename().string().c_str(), ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(path.filename().string().c_str(), ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		throw std::runtime_error{ std::format("Invalid shader format.") };
	}


	size_t vertex_code_size;
	std::unique_ptr<void, VertexCodeDeleter> vertex_code{ SDL_LoadFile(path.string().c_str(), &vertex_code_size) };

	if (!vertex_code)
	{
		throw std::runtime_error{ std::format("Failed to load shared: {}", SDL_GetError()) };
	}

	SDL_GPUShaderCreateInfo vertex_info;
	vertex_info.code = static_cast<uint8_t*>(vertex_code.get());
	vertex_info.code_size = vertex_code_size;
	vertex_info.entrypoint = "main";
	vertex_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
	vertex_info.stage = stage;
	vertex_info.num_samplers = sampler_count;
	vertex_info.num_storage_buffers = 0;
	vertex_info.num_storage_textures = 0;
	vertex_info.num_uniform_buffers = uniform_count;
	shader = SDL_CreateGPUShader(device.get(), &vertex_info);

	if (!shader)
	{
		throw std::runtime_error{ std::format("Failed to create GPU shader: {}", SDL_GetError()) };
	}
}

graphics::GpuShader::~GpuShader()
{
	if (shader)
		SDL_ReleaseGPUShader(device.get(), shader);
}
