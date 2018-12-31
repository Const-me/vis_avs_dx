#include "stdafx.h"
#include "../EffectImpl.hpp"
#include "Blender.h"

class BufferSave : public EffectBase
{
public:
	enum eDirection : int
	{
		Save = 0,
		Restore = 1,
		SaveRestore = 2,
		RestoreSave = 3,
	};

	struct AvsState
	{
		int clear;
		volatile int dir_ch;
		int blend;
		eDirection dir;
		int which;
		int adjblend_val;
	};

	BufferSave( AvsState* pState ) : avs( *pState ) { }

private:

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	AvsState& avs;
	Blender m_blend;
};

IMPLEMENT_EFFECT( BufferSave, C_StackClass )

constexpr float div255 = 1.0f / 255.0f;

HRESULT BufferSave::render( bool isBeat, RenderTargets& rt )
{
	RenderTargets& target = StaticResources::globalBuffers[ avs.which ];
	if( avs.clear )
	{
		avs.clear = 0;
		if( !target.lastWritten() )
			CHECK( target.lastWritten().create() );
		target.lastWritten().clear();
	}

	eDirection dir;
	if( avs.dir < 2 )
		dir = avs.dir;
	else
	{
		dir = (eDirection)( ( avs.dir & 1 ) ^ avs.dir_ch );
		avs.dir_ch ^= 1;
	}

	RenderTargets &source = ( dir == eDirection::Save ) ? rt : target;
	RenderTargets &dest = ( dir == eDirection::Save ) ? target : rt;
	if( !source.lastWritten() )
		return S_FALSE;

	eBlendMode mode;
	float val = 0;
	switch( avs.blend )
	{
	case 0:
	default:
		mode = eBlendMode::Replace;
		break;
	case 1:
		mode = eBlendMode::Fifty;
		break;
	case 2:
		mode = eBlendMode::Additive;
		break;
	case 3:
		mode = eBlendMode::PixelInterleave;
		break;
	case 4:
		mode = eBlendMode::LineInterleave;
		break;
	case 5:
		mode = eBlendMode::Subtractive1;
		break;
	case 6:
		mode = eBlendMode::Subtractive;
		break;
	case 7:
		mode = eBlendMode::Xor;
		break;
	case 8:
		mode = eBlendMode::Maximum;
		break;
	case 9:
		mode = eBlendMode::Minimum;
		break;
	case 10:
		mode = eBlendMode::Multiply;
		break;
	case 11:
		mode = eBlendMode::Adjustable;
		val = div255 * (float)avs.adjblend_val;
		break;
	}

	return m_blend.blend( source, dest, mode, val );
}