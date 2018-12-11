#pragma once

enum struct eStage : uint8_t
{
	Vertex = 0,
	Pixel = 1,
	Geometry = 2,
	Compute = 3,
};

namespace
{
	template<eStage stage>
	struct ShaderTraits;

	template<>
	struct ShaderTraits<eStage::Vertex>
	{
		using IShader = ID3D11VertexShader;
		static constexpr const char * shaderName = "vertex";
	};

	template<>
	struct ShaderTraits<eStage::Pixel>
	{
		using IShader = ID3D11PixelShader;
		static constexpr const char * shaderName = "pixel";
	};

	template<>
	struct ShaderTraits<eStage::Geometry>
	{
		using IShader = ID3D11GeometryShader;
		static constexpr const char * shaderName = "geometry";
	};

	template<>
	struct ShaderTraits<eStage::Compute>
	{
		using IShader = ID3D11ComputeShader;
		static constexpr const char * shaderName = "compute";
	};
}

template<eStage stage>
using IShader = typename ShaderTraits<stage>::IShader;

template<eStage stage>
using ShaderPtr = CComPtr<typename ShaderTraits<stage>::IShader>;

template<eStage stage>
constexpr const char* shaderName()
{
	return ShaderTraits<stage>::shaderName;
}