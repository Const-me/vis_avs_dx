#include "stdafx.h"
#include "EffectList.h"

eBlendMode EffectList::blendin() const
{
	return (eBlendMode)( ( avs->mode >> 8 ) & 31 );
}

constexpr float div255 = 1.0f / 255.0f;

float EffectList::inblendval() const
{
	return div255 * avs->inblendval;
}

eBlendMode EffectList::blendout() const
{
	return (eBlendMode)( ( ( avs->mode >> 16 ) & 31 ) ^ 1 );
}

float EffectList::outblendval() const
{
	return div255 * avs->outblendval;
}

HRESULT EffectList::render( bool isBeat, RenderTargets& rt )
{
	const int enabled = ( avs->mode & 2 ) ^ 2;
	if( !enabled )
		return S_FALSE;

	const eBlendMode blendIn = blendin();
	if( clearfb() && blendIn != eBlendMode::Replace && m_rt.lastWritten() )
		m_rt.lastWritten().clear();

	CHECK( m_blendIn.blend( rt, m_rt, blendIn, inblendval() ) );

	CHECK( EffectListBase::render( isBeat, m_rt ) );

	CHECK( m_blendOut.blend( m_rt, rt, blendout(), outblendval() ) );

	return S_OK;
}