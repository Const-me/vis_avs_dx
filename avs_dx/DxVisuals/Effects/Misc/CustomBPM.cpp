#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>

class CustomBPM : public EffectBase
{
public:
	using fnCustomBpm = int( *)( void* pThis, bool isBeat );

	// No point to copy-paste then debug the original code when we can just call it: it doesn't render anything.
	struct AvsState
	{
		void* pThis;
		fnCustomBpm pfnProcess;
	};

	CustomBPM( AvsState* pState ) :
		avs( *pState )
	{ }

	DECLARE_EFFECT()

private:
	AvsState &avs;
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( CustomBPM, C_BpmClass );

#define SET_BEAT		0x10000000
#define CLR_BEAT		0x20000000

HRESULT CustomBPM::render( bool isBeat, RenderTargets& rt )
{
	const int result = avs.pfnProcess( avs.pThis, isBeat );
	switch( result )
	{
	case CLR_BEAT:
		return S_CLEAR_BEAT;
	case SET_BEAT:
		return S_SET_BEAT;
	}
	return S_FALSE;
}