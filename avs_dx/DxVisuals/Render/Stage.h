#pragma once

enum struct eStage : uint8_t
{
	Compute = 0,
	Vertex = 1,
	Geometry = 2,
	Pixel = 3,
};

namespace DxUtils
{
	// Abuse C macros + C++ template specialization to implement a poor man's compile time reflection..
	template<eStage stage>
	struct StageBinder;

#define BIND_SHADER( STAGE )    static inline void shader( IShader* shaderPtr ) { context->STAGE##SetShader( shaderPtr, nullptr, 0 ); }
#define BIND_CONSTANT( STAGE )  static inline void cbuffer( UINT slot, ID3D11Buffer* buffer ) { context->STAGE##SetConstantBuffers( slot, 1, &buffer ); }
#define BIND_SRV( STAGE )       static inline void srv( UINT slot, ID3D11ShaderResourceView* view ) { context->STAGE##SetShaderResources( slot, 1, &view ); }
#define BIND_SAMPLER( STAGE )   static inline void sampler( UINT slot, ID3D11SamplerState* sampler ) { context->STAGE##SetSamplers( slot, 1, &sampler ); }
#define IMPLEMENT_BINDS( STAGE ) BIND_SHADER( STAGE ) BIND_CONSTANT( STAGE ) BIND_SRV( STAGE ) BIND_SAMPLER( STAGE )

	template<>
	struct StageBinder<eStage::Vertex>
	{
		using IShader = ID3D11VertexShader;
		IMPLEMENT_BINDS( VS )
	};

	template<>
	struct StageBinder<eStage::Pixel>
	{
		using IShader = ID3D11PixelShader;
		IMPLEMENT_BINDS( PS )
	};

	template<>
	struct StageBinder<eStage::Geometry>
	{
		using IShader = ID3D11GeometryShader;
		IMPLEMENT_BINDS( GS )
	};

	template<>
	struct StageBinder<eStage::Compute>
	{
		using IShader = ID3D11ComputeShader;
		IMPLEMENT_BINDS( CS )
	};

#undef IMPLEMENT_BINDS
#undef BIND_SAMPLER
#undef BIND_SRV
#undef BIND_CONSTANT
#undef BIND_SHADER
}

template<eStage stage>
using IShader = typename DxUtils::StageBinder<stage>::IShader;

template<eStage stage>
using ShaderPtr = CComPtr<typename DxUtils::StageBinder<stage>::IShader>;

// Bind a shader
template<eStage stage>
inline void bindShader( IShader<stage>* ptr )
{
	DxUtils::StageBinder<stage>::shader( ptr );
}
// Bind a constant buffers
template<eStage stage>
inline void bindConstantBuffer( UINT slot, ID3D11Buffer* buffer = nullptr )
{
	DxUtils::StageBinder<stage>::cbuffer( slot, buffer );
}
// Bind a shader resource
template<eStage stage>
inline void bindResource( UINT slot, ID3D11ShaderResourceView* srv = nullptr )
{
	DxUtils::StageBinder<stage>::srv( slot, srv );
}
// Bind a sampler
template<eStage stage>
inline void bindSampler( UINT slot, ID3D11SamplerState* sampler = nullptr )
{
	DxUtils::StageBinder<stage>::sampler( slot, sampler );
}

// UAVs are only supported by compute shaders
inline void bindUav( UINT slot, ID3D11UnorderedAccessView* uav = nullptr )
{
	context->CSSetUnorderedAccessViews( slot, 1, &uav, nullptr );
}