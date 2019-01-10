#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>

class SetRenderMode : public EffectBase
{
public:
	struct AvsState
	{
		int newmode;
	};

	SetRenderMode( AvsState* pState ) :
		avs( *pState )
	{ }

	DECLARE_EFFECT()

private:
	AvsState &avs;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( SetRenderMode, C_LineModeClass )

extern int g_line_blend_mode;

HRESULT SetRenderMode::render( bool isBeat, RenderTargets& rt )
{
	if( avs.newmode & 0x80000000 )
		g_line_blend_mode = avs.newmode & 0x7fffffff;
	return S_FALSE;
}