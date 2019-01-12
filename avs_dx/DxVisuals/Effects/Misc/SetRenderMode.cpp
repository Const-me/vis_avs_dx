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

	bool apply();

	// Different effects use the value on both passes, update and rendering

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( SetRenderMode, C_LineModeClass )

extern int g_line_blend_mode;

bool SetRenderMode::apply()
{
	if( 0 == ( avs.newmode & 0x80000000 ) )
		return false;	// Not enabled

	const int newMode = avs.newmode & 0x7fffffff;
	if( newMode == g_line_blend_mode )
		return false;	// Same value

	g_line_blend_mode = newMode;
	return true;
}

HRESULT SetRenderMode::updateParameters( Binder& binder )
{
	apply();
	return S_FALSE;
}

HRESULT SetRenderMode::render( bool isBeat, RenderTargets& rt )
{
	apply();
	return S_FALSE;
}