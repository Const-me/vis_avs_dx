#pragma once

enum struct eStage : uint8_t
{
	Compute = 0,
	Vertex = 1,
	Geometry = 2,
	Pixel = 3,
};

namespace
{
	template<eStage stage>
	struct ShaderTraits;

	template<>
	struct ShaderTraits<eStage::Vertex>
	{
		using IShader = ID3D11VertexShader;
		static inline void bind( IShader* ptr )
		{
			context->VSSetShader( ptr, nullptr, 0 );
		}
	};

	template<>
	struct ShaderTraits<eStage::Pixel>
	{
		using IShader = ID3D11PixelShader;
		static inline void bind( IShader* ptr )
		{
			context->PSSetShader( ptr, nullptr, 0 );
		}
	};

	template<>
	struct ShaderTraits<eStage::Geometry>
	{
		using IShader = ID3D11GeometryShader;
		static inline void bind( IShader* ptr )
		{
			context->GSSetShader( ptr, nullptr, 0 );
		}
	};

	template<>
	struct ShaderTraits<eStage::Compute>
	{
		using IShader = ID3D11ComputeShader;
		static inline void bind( IShader* ptr )
		{
			context->CSSetShader( ptr, nullptr, 0 );
		}
	};
}

template<eStage stage>
using IShader = typename ShaderTraits<stage>::IShader;

template<eStage stage>
using ShaderPtr = CComPtr<typename ShaderTraits<stage>::IShader>;

template<eStage stage>
inline void bindShader( IShader<stage>* ptr )
{
	ShaderTraits<stage>::bind( ptr );
}