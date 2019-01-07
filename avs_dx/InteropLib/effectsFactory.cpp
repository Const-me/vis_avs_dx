#include "stdafx.h"
#include "effectsFactory.h"
#include <../DxVisuals/effects.h>

CComAutoCriticalSection g_effectsLock;

CAtlMap<const void*, unique_ptr<EffectBase>> g_effects{ 17, 0.75f, 0.05f, 1.25f, 32 };	// It has very few items: we don't care about RAM but want to minimize rehashes and collisions.

void addNewEffect( const C_RBASE *pThis, unique_ptr<EffectBase>&& fx )
{
	assert( nullptr != pThis );

	CSLock __lock( g_effectsLock );
	auto& up = g_effects[ pThis ];
	assert( !up.operator bool() );
	up.swap( fx );
}

void createEmptyEffect( const C_RBASE* pThis )
{
	assert( nullptr != pThis );

	CSLock __lock( g_effectsLock );
	auto& up = g_effects[ pThis ];
	assert( !up.operator bool() );
}

bool hasDxEffect( const C_RBASE* pThis )
{
	if( nullptr == pThis )
		return false;

	CSLock __lock( g_effectsLock );
	return nullptr != g_effects.Lookup( pThis );
}

void destroyDxEffect( const C_RBASE* pThis )
{
	if( nullptr == pThis )
		return;

	CSLock __lock( g_effectsLock );
	g_effects.RemoveKey( pThis );
}

EffectBase* getDxEffect( const C_RBASE* pThis )
{
	if( nullptr == pThis )
		return nullptr;

	CSLock __lock( g_effectsLock );
	auto p = g_effects.Lookup( pThis );
	if( nullptr == p )
		return nullptr;

	return p->m_value.get();
}

void destroyAllEffects()
{
	CSLock __lock( g_effectsLock );
	g_effects.RemoveAll();
}