#include "stdafx.h"
#include "EffectList.h"

int EffectList::blendin() const
{
	return ( ( avs->mode >> 8 ) & 31 );
}

constexpr float div255 = 1.0f / 255.0f;

float EffectList::inblendval() const
{
	return div255 * avs->inblendval;
}

int EffectList::blendout() const
{
	return ( ( avs->mode >> 16 ) & 31 ) ^ 1;
}

float EffectList::outblendval() const
{
	return div255 * avs->outblendval;
}

HRESULT EffectList::render( RenderTargets& rt )
{
	CHECK( m_blendIn.blend( rt, m_rt, blendin(), inblendval() ) );

	CHECK( EffectListBase::render( m_rt ) );

	CHECK( m_blendOut.blend( m_rt, rt, blendout(), outblendval() ) );

	return S_OK;
}