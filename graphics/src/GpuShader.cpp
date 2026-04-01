#include "GpuShader.hpp"
#include <SDL3_shadercross/SDL_shadercross.h>

graphics::GpuShader::GpuShader(std::shared_ptr<SDL_GPUDevice> device, const std::filesystem::path& path, int sampler_count, int uniform_count, int storage_buffer_count, int storage_texture_count)
	: device{ device }
{
	SDL_ShaderCross_ShaderStage stage;

	if (SDL_strstr(path.filename().string().c_str(), ".vert"))
	{
		stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(path.filename().string().c_str(), ".frag"))
	{
		stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		throw std::runtime_error{ std::format("Invalid shader format.") };
	}


	size_t vertex_code_size;
	std::unique_ptr<void, VertexCodeDeleter> hlsl_vertex_code{ SDL_LoadFile(path.string().c_str(), &vertex_code_size) };

	if (!hlsl_vertex_code)
	{
		throw std::runtime_error{ std::format("Failed to load shader: {}", SDL_GetError()) };
	}

	SDL_ShaderCross_HLSL_Info vertex_info = {};
	std::string source {static_cast<char*>(hlsl_vertex_code.get()), vertex_code_size};
	vertex_info.source = source.c_str();
	vertex_info.entrypoint = "main";
	vertex_info.include_dir = nullptr;
	vertex_info.defines = nullptr;
	vertex_info.shader_stage = stage;
	
	std::unique_ptr<void, VertexCodeDeleter> spirv_vertex_code { SDL_ShaderCross_CompileSPIRVFromHLSL(&vertex_info, &vertex_code_size) };

	SDL_ShaderCross_SPIRV_Info spirv_info = {};
	spirv_info.bytecode = static_cast<Uint8*>(spirv_vertex_code.get());
	spirv_info.bytecode_size = vertex_code_size;
	spirv_info.entrypoint = "main";
	spirv_info.shader_stage = stage;

	SDL_ShaderCross_GraphicsShaderResourceInfo shader_resource_info = {};
	shader_resource_info.num_samplers = sampler_count;
	shader_resource_info.num_storage_buffers = storage_buffer_count;
	shader_resource_info.num_storage_textures = storage_texture_count;
	shader_resource_info.num_uniform_buffers = uniform_count;

	shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(device.get(), &spirv_info, &shader_resource_info, 0);

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
