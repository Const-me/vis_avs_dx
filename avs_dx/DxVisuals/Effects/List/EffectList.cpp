#include "stdafx.h"
#include "EffectList.h"

eBlendMode EffectList::blendin() const
{
	return (eBlendMode)( ( avs->mode >> 8 ) & 31 );
}

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

HRESULT EffectList::updateParameters( Binder& binder )
{
	BoolHr hr;
	if( Blender::modeUsesShader( blendin() ) )
		hr.combine( m_blendIn.updateBindings( binder ) );

	hr.combine( __super::updateParameters( binder ) );

	if( Blender::modeUsesShader( blendout() ) )
		hr.combine( m_blendOut.updateBindings( binder ) );

	return hr;
}

class SavedContext
{
	const int savedBlendMode;
	GlobalBuffers& savedBuffers;

	static void swapBuffers( GlobalBuffers& a, GlobalBuffers& b )
	{
		constexpr size_t cb = sizeof( GlobalBuffers );
		array<uint8_t, cb> tmp;
		CopyMemory( tmp.data(), &a, cb );	// tmp = a
		CopyMemory( &a, &b, cb );			// a = b
		CopyMemory( &b, tmp.data(), cb );	// b = tmp
	}

public:
	SavedContext( GlobalBuffers& buffers ) :
		savedBuffers( buffers ),
		savedBlendMode( g_line_blend_mode )
	{
		swapBuffers( savedBuffers, StaticResources::globalBuffers );
	}

	~SavedContext()
	{
		g_line_blend_mode = savedBlendMode;
		swapBuffers( savedBuffers, StaticResources::globalBuffers );
	}
};

HRESULT EffectList::render( bool isBeat, RenderTargets& rt )
{
	const int enabled = ( avs->mode & 2 ) ^ 2;
	if( !enabled )
		return S_FALSE;

	SavedContext __context{ m_savedGlobalBuffers };

	const eBlendMode blendIn = blendin();
	const bool clearBuffer = clearfb();
	if( blendIn == eBlendMode::Buffer )
	{
		CHECK( m_blendIn.blend( StaticResources::globalBuffers[ avs->bufferin ], m_rt, eBlendMode::Replace, inblendval() ) );
	}
	else
	{
		if( clearBuffer && blendIn != eBlendMode::Replace && m_rt.lastWritten() )
			m_rt.lastWritten().clear();

		if( !clearBuffer && blendIn == eBlendMode::Ignore )
		{
			CHECK( fadeRenderTarget( m_rt ) );
		}
		else
			CHECK( m_blendIn.blend( rt, m_rt, blendIn, inblendval() ) );
	}

	CHECK( EffectListBase::render( isBeat, m_rt ) );

	const eBlendMode blendOut = blendout();
	if( blendOut == eBlendMode::Buffer )
	{
		CHECK( m_blendOut.blend( m_rt, StaticResources::globalBuffers[ avs->bufferout ], eBlendMode::Replace, outblendval() ) );
	}
	else
	{
		CHECK( m_blendOut.blend( m_rt, rt, blendout(), outblendval() ) );
	}

	return S_OK;
}