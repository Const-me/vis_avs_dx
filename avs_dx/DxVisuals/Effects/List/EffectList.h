#pragma once
#include "../Common/EffectListBase.h"
#include "Blender.h"
#include <Resources/RenderTargets.h>

class EffectList : public EffectListBase
{
	RenderTargetsAutoDrop m_rt;
	Blender m_blendIn, m_blendOut;

	eBlendMode blendin() const;
	float inblendval() const;
	eBlendMode blendout() const;
	float outblendval() const;

	GlobalBuffers m_savedGlobalBuffers;

public:

	EffectList( AvsState* pState ) : EffectListBase( pState ) { }

	DECLARE_EFFECT()

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};