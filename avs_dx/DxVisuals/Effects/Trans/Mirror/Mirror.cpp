#pragma once
#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>
#include <Resources/dynamicBuffers.h>
#include <Utils/LinearTransition.hpp>

struct MirrorStructs
{
	// The content of constant buffer that's passed to the PS.
	struct MirrorCb
	{
		float2 amounts;
		BOOL transitionFlag;
		uint zzUnused = 0;
	};

	struct AvsState
	{
		int enabled;
		int mode;
		int onbeat;
		int rbeat;
		int smooth;
		// Transition speed between 1 and 16, 16 = slowest
		int slower;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Hlsl::Trans::Mirror::MirrorPS;
};

class Mirror : public EffectBase1<MirrorStructs>
{
	int m_mode = -1;
	LinearTransition<Vector2> m_trans;
	CComPtr<ID3D11Buffer> m_cb;
	MirrorCb update( bool isBeat );

public:
	Mirror( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	HRESULT buildState( EffectStateShader& ess ) override
	{
		avs->rbeat = avs->mode;
		return __super::buildState( ess );
	}

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( Mirror, C_MirrorClass )

HRESULT Mirror::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	MirrorCb cb = update( isBeat );
	CHECK( updateCBuffer( m_cb, &cb, sizeof( cb ) ) );
	bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );	//< Don't unbind. The Binder class allocates unique slots to effects, so in runtime the buffer remains bound to that slot but that doesn't break other effects.

	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );

	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}

inline float getAmount( int mode )
{
	if( 0 == mode )
		return 0;
	if( 1 & mode )
		return +1;
	return -1;
}

inline Vector2 getAmounts( int mode )
{
	return Vector2{ getAmount( ( mode >> 2 ) & 3 ), getAmount( mode & 3 ) };
}

Mirror::MirrorCb Mirror::update( bool isBeat )
{
	const int *pThisMode = &avs->mode;
	if( avs->onbeat )
	{
		if( isBeat )
			avs->rbeat = ( rand() % 16 ) & avs->mode;
		pThisMode = &avs->rbeat;
	}

	const int mode = *pThisMode;
	if( mode != m_mode )
	{
		m_mode = mode;
		m_trans.setTarget( avs->slower * 4, getAmounts( m_mode ) );
	}

	if( !avs->smooth )
	{
		// Instant transitions
		return MirrorCb{ m_trans.target(), FALSE };
	}

	if( m_trans.finished() )
	{
		// Smooth transition has finished
		return MirrorCb{ m_trans.target(), FALSE };
	}

	return MirrorCb{ m_trans.update(), TRUE };
}