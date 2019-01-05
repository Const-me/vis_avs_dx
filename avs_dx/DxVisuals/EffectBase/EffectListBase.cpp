#include "stdafx.h"
#include "EffectListBase.h"
#include <Effects/shadersCode.h>

EffectListBase::EffectListBase( AvsState* pState ) : 
	avs( pState ),
	m_profilerStart( "ListBegin" )
{ }

EffectBase* EffectListBase::T_RenderListType::dxEffect() const
{
	return getDxEffect( render );
}

bool EffectListBase::updateList( RootEffect* pRoot )
{
	bool result = false;
	size_t nextId = 0;
	const size_t size = m_effects.size();
	for( int i = 0; i < avs->num_renders; i++ )
	{
		T_RenderListType& item = avs->renders[ i ];
		if( !item.hasDxEffect() )
			continue;
		EffectBase* const pEffect = item.dxEffect();
		pEffect->setOwner( pRoot );

		if( nextId >= size )
		{
			// The new list is longer.
			result = true;
			m_effects.push_back( pEffect );
		}
		else if( m_effects[ nextId ] != pEffect )
		{
			// Another effect at that index
			result = true;
			m_effects[ nextId ] = pEffect;
		}
		nextId++;

		if( !pEffect->metadata().isList )
			continue;
		EffectListBase* pList = static_cast<EffectListBase*>( pEffect );
		if( pList->updateList( pRoot ) )
			result = true;
	}

	if( nextId != m_effects.size() )
	{
		// The new list is shorter.
		result = true;
		m_effects.resize( nextId );
	}

	return result;
}

HRESULT EffectListBase::shouldRebuildState()
{
	BoolHr hr;
	for( auto p : m_effects )
	{
		if( hr.combine( p->shouldRebuildState() ) )
			break;
	}
	return hr;
}

HRESULT EffectListBase::updateParameters( Binder& binder )
{
	return apply( [ &binder ]( EffectBase &e ) { return e.updateParameters( binder ); } );
}

HRESULT EffectListBase::render( bool isBeat, RenderTargets& rt )
{
	m_profilerStart.mark();
	
	HRESULT hr = E_UNEXPECTED;
	{
		ProfilerLevel plvl;
		hr = apply( [ &isBeat, &rt ]( EffectBase &e )
		{
			const HRESULT hr = e.render( isBeat, rt );
			switch( hr )
			{
			case S_OK:
				e.completedRendering();
				return S_OK;
			case S_CLEAR_BEAT:
				isBeat = false;
				return S_FALSE;
			case S_SET_BEAT:
				isBeat = true;
				return S_FALSE;
			}
			return hr;
		} );
	}

	completedRendering();
	return hr;
}

HRESULT EffectListBase::fadeRenderTarget( RenderTargets &rt )
{
	setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::fadeFramePS );
	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );
	drawFullscreenTriangle( false );
	return S_OK;
}