#pragma once
#include "../../EffectBase/EffectListBase.h"
#include "Blender.h"
#include <Resources/RenderTargets.h>

class EffectList : public EffectListBase
{
	RenderTargets m_rt;
	Blender m_blendIn, m_blendOut;

	eBlendMode blendin() const;
	float inblendval() const;
	eBlendMode blendout() const;
	float outblendval() const;

public:

	EffectList( AvsState* pState ) : EffectListBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override;
};