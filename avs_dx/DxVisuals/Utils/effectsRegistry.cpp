#include "stdafx.h"
#include "effectsRegistry.h"

namespace
{
	CComAutoCriticalSection g_cs;
	using EffectPtr = std::unique_ptr<iRootEffect>;
	CAtlMap<EffectPtr*, bool> g_map{ 17, 0.75f, 0.05f, 1.25f, 32 };	// It has very few items: we don't care about RAM but want to minimize rehashes and collisions.
}

void registerEffect( EffectPtr* p )
{
	CSLock __lock( g_cs );
#ifdef DEBUG
	assert( nullptr == g_map.Lookup( p ) );
#endif
	g_map.SetAt( p, true );
}

void unregisterEffect( EffectPtr* p )
{
	CSLock __lock( g_cs );
	g_map.RemoveKey( p );
}

int destroyEffects()
{
	CSLock __lock( g_cs );
	int res = 0;
	for( POSITION pos = g_map.GetStartPosition(); pos;)
	{
		EffectPtr* p = g_map.GetNextKey( pos );
		p->reset();
		res++;
	}
	g_map.RemoveAll();
	return res;
}