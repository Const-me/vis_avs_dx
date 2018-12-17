#include "stdafx.h"
#include "EffectListBase.h"

EffectBase* EffectListBase::T_RenderListType::dxEffect() const
{
	if( !render->dxEffect )
		return nullptr;
	iRootEffect* p = render->dxEffect.get();
	return static_cast<EffectBase*>( p );
}

bool EffectListBase::updateList()
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
		if( pList->updateList() )
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
	HRESULT hr = S_FALSE;
	for( auto p : m_effects )
	{
		const HRESULT r = p->shouldRebuildState();
		CHECK( r );
		if( S_OK == r )
			hr = S_OK;
	}
	return hr;
}

HRESULT EffectListBase::updateParameters( Binder& binder )
{
	return apply( [ &binder ]( EffectBase &e ) { return e.updateParameters( binder ); } );
}

HRESULT EffectListBase::render( RenderTargets& rt )
{
	return apply( [ &rt ]( EffectBase &e ) { return e.render( rt ); } );
}