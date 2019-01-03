#pragma once
#include "../EffectBase1.hpp"
#include "../EffectProfiler.h"
#include "PointSpritesRender.hpp"

class ParticleSystemBase
{
	// Compute shader threads per group. Both compute shaders must have [numthreads( csThreads, 1, 1 )]
	static constexpr UINT csThreads = 256;

	const UINT particleByteWidth;
	UINT m_particles, m_groups;
	EffectProfiler m_profiler;

public:

	const UINT particlesCapacity;
	StructureBuffer m_buffer;

	ParticleSystemBase( UINT cb, UINT n, const CAtlMap<CStringA, CStringA>& inc, const char* updateProfilerName ) :
		particleByteWidth( cb ), particlesCapacity( n ),
		m_profiler( updateProfilerName )
	{ }

	HRESULT ensureBuffer()
	{
		if( m_buffer )
			return S_FALSE;
		CHECK( m_buffer.create( particleByteWidth, particlesCapacity ) );
		return S_OK;
	}

	void dispatch( UINT count )
	{
		context->Dispatch( ( count + csThreads - 1 ) / csThreads, 1, 1 );
	}

	template<class TRenderer>
	HRESULT renderParticles( TRenderer& renderer, UINT count )
	{
		// Update particles positions with the CS
		{
			auto bound = boundResource( renderer.compute().bindParticles, m_buffer.uav() );
			count = std::min( count, particlesCapacity );
			dispatch( count );
			m_profiler.mark();
		}

		// Render the sprites
		{
			auto bound = boundResource<eStage::Vertex>( renderer.vertex().bindParticles, m_buffer.srv() );
			iaClearBuffer();
			context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
			context->Draw( count, 0 );
		}
		return S_OK;
	}
};

template<class TStruct>
class FacticleFx : public EffectBase1<TStruct>, protected ParticleSystemBase
{
	// Extra compute shader to initialize the particle system
	Shader<typename TStruct::InitCsData> m_initShader;
	bool m_needReinit = true;

protected:

	FacticleFx( typename TStruct::AvsState *pState, UINT cb, UINT n, const char* updateProfilerName ) :
		EffectBase1<TStruct>( pState ),
		ParticleSystemBase( cb, n, TStruct::effectIncludes(), updateProfilerName )
	{ }

	HRESULT updateParameters( Binder& binder ) override
	{
		BoolHr hr = __super::updateParameters( binder );

		{	
			BoolHr hrInit = m_initShader.update( binder, nullptr, nullptr );
			if( hrInit.succeeded() && hrInit.value() )
				hrInit.combine( m_initShader.compile( TStruct::effectIncludes(), EffectBase::stateOffset() ) );
			hr.combine( hrInit );

		}

		if( m_needReinit && hr.succeeded() && m_initShader.hasShader() )
		{
			m_needReinit = false;
			m_initShader.bind( false );
			auto bound = boundResource( m_initShader.data().bindParticles, m_buffer.uav() );
			dispatch( particlesCapacity );
		}

		return hr;
	}

	HRESULT initializedState() override
	{
		CHECK( __super::initializedState() );
		CHECK( ParticleSystemBase::ensureBuffer() );
		m_needReinit = true;
		return S_OK;
	}

	HRESULT renderParticles( UINT count )
	{
		return ParticleSystemBase::renderParticles( EffectBase1<TStruct>::renderer, count );
	}
};