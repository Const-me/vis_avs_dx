#pragma once
#include "EffectBase.h"
#include "../Render/EffectStateBuilder.h"

// Interop with the AVS code
inline int min( int a, int b )
{
	return std::min( a, b );
}
inline int max( int a, int b )
{
	return std::max( a, b );
}
#include "../../avs/vis_avs/r_defs.h"
#include "../InteropLib/effectsFactory.h"

template<class TEffect>
class EffectImpl : public TEffect
{
public:
	EffectImpl( void* pNative ) : TEffect( ( typename TEffect::AvsState* )( pNative ) ) { }

	static inline HRESULT create( void* pState, std::unique_ptr<EffectBase>& res )
	{
		using tImpl = EffectImpl<TEffect>;
		res = std::make_unique<tImpl>( pState );
		return S_OK;
	}
};

#define DECLARE_EFFECT( DX ) struct AvsState; AvsState* const avs; DX( AvsState* pState ) : avs( pState ) { }
#define IMPLEMENT_EFFECT( DX, NATIVE ) class NATIVE; template<> HRESULT createDxEffect<NATIVE>( void* pState, std::unique_ptr<EffectBase>& dest ) { return EffectImpl<DX>::create( pState, dest ); };