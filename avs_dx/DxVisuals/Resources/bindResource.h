#pragma once
#include "../Render/Stage.h"

namespace DxUtils
{
	struct ConstantBufferResource
	{
		template<eStage stage>
		static inline void bind( UINT slot, ID3D11Buffer* buffer )
		{
			StageBinder<stage>::cbuffer( slot, buffer );
		}
		using IResource = ID3D11Buffer;
	};
	struct ShaderResource
	{
		template<eStage stage>
		static inline void bind( UINT slot, ID3D11ShaderResourceView* view )
		{
			StageBinder<stage>::srv( slot, view );
		}
		using IResource = ID3D11ShaderResourceView;
	};
	struct SamplerResource
	{
		template<eStage stage>
		static inline void bind( UINT slot, ID3D11SamplerState* view )
		{
			StageBinder<stage>::sampler( slot, view );
		}
		using IResource = ID3D11SamplerState;
	};

	// RAII wrapper with bound resource.
	template<eStage stage, class TResource>
	class BoundResource
	{
		UINT m_slot;

	public:
		BoundResource() : m_slot( UINT_MAX ) { }
		BoundResource( BoundResource &that ) = delete;
		void operator=( BoundResource &that ) = delete;
		BoundResource( BoundResource &&that ) : m_slot( that.m_slot )
		{
			that.m_slot = UINT_MAX;
		}

		~BoundResource()
		{
			if( m_slot != UINT_MAX )
				TResource::bind<stage>( m_slot, nullptr );
		}
		BoundResource( UINT slot, typename TResource::IResource* res )
		{
			TResource::bind<stage>( slot, res );
			m_slot = ( nullptr != res ) ? slot : UINT_MAX;
		}
		void swap( BoundResource<stage, TResource> &that )
		{
			assert( m_slot == UINT_MAX || that.m_slot == UINT_MAX );
			std::swap( m_slot, that.m_slot );
		}
		void swap( BoundResource<stage, TResource> &&that )
		{
			assert( m_slot == UINT_MAX || that.m_slot == UINT_MAX );
			std::swap( m_slot, that.m_slot );
		}
	};
}

template<eStage stage>
using BoundSrv = DxUtils::BoundResource<stage, DxUtils::ShaderResource>;
using BoundPsResource = BoundSrv<eStage::Pixel>;

#define BIND_CS_SRV( slot, srv ) BoundSrv<eStage::Compute> _bind_cs_srv_##slot{ slot, srv }
#define BIND_VS_SRV( slot, srv ) BoundSrv<eStage::Vertex> _bind_vs_srv_##slot{ slot, srv }
#define BIND_GS_SRV( slot, srv ) BoundSrv<eStage::Geometry> _bind_gs_srv_##slot{ slot, srv }
#define BIND_PS_SRV( slot, srv ) BoundSrv<eStage::Pixel> _bind_ps_srv_##slot{ slot, srv }

template<eStage stage>
using BoundCBuffer = DxUtils::BoundResource<stage, DxUtils::ConstantBufferResource>;

#define BIND_CS_CB( slot, pBuffer ) BoundCBuffer<eStage::Compute> _bind_cs_cb_##slot{ slot, pBuffer }
#define BIND_VS_CB( slot, pBuffer ) BoundCBuffer<eStage::Vertex> _bind_vs_cb_##slot{ slot, pBuffer }
#define BIND_GS_CB( slot, pBuffer ) BoundCBuffer<eStage::Geometry> _bind_gs_cb_##slot{ slot, pBuffer }
#define BIND_PS_CB( slot, pBuffer ) BoundCBuffer<eStage::Pixel> _bind_ps_cb_##slot{ slot, pBuffer }

template<eStage stage>
using BoundSampler = DxUtils::BoundResource<stage, DxUtils::SamplerResource>;

#define BIND_CS_SAMPLER( slot, pSampler ) BoundSampler<eStage::Compute> _bind_cs_sam_##slot{ slot, pSampler }
#define BIND_VS_SAMPLER( slot, pSampler ) BoundSampler<eStage::Vertex> _bind_vs_sam_##slot{ slot, pSampler }
#define BIND_GS_SAMPLER( slot, pSampler ) BoundSampler<eStage::Geometry> _bind_gs_sam_##slot{ slot, pSampler }
#define BIND_PS_SAMPLER( slot, pSampler ) BoundSampler<eStage::Pixel> _bind_ps_sam_##slot{ slot, pSampler }

// Bind the resource to the same slot to all 4 stages
inline void bindGlobalResource( UINT slot, ID3D11ShaderResourceView* srv = nullptr )
{
	bindResource<eStage::Compute>( slot, srv );
	bindResource<eStage::Vertex>( slot, srv );
	bindResource<eStage::Geometry>( slot, srv );
	bindResource<eStage::Pixel>( slot, srv );
}

inline void bindSampler( UINT slot, ID3D11SamplerState* s = nullptr )
{
	context->CSSetSamplers( slot, 1, &s );
	context->VSSetSamplers( slot, 1, &s );
	context->GSSetSamplers( slot, 1, &s );
	context->PSSetSamplers( slot, 1, &s );
}

template<eStage stage = eStage::Pixel>
BoundSrv<stage> boundResource( UINT slot, ID3D11ShaderResourceView* srv )
{
	return std::move( BoundSrv<stage>{ slot, srv } );
}