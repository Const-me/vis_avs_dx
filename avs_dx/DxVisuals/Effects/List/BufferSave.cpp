#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>
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

	HRESULT updateParameters( Binder& binder );

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	AvsState& avs;
	Blender m_blend;
};

IMPLEMENT_EFFECT( BufferSave, C_StackClass )

constexpr float div255 = 1.0f / 255.0f;

HRESULT BufferSave::updateParameters( Binder& binder )
{
	return m_blend.updateBindings( binder ) ? S_OK : S_FALSE;
}

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

	/*
CONTROL         "Replace",IDC_RSTACK_BLEND1,"Button",BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP,12,79,43,10
CONTROL         "50/50",IDC_RSTACK_BLEND2,"Button",BS_AUTORADIOBUTTON,12,89,35,10
CONTROL         "Additive",IDC_RSTACK_BLEND3,"Button",BS_AUTORADIOBUTTON,12,99,41,10
CONTROL         "Every other pixel",IDC_RSTACK_BLEND4,"Button",BS_AUTORADIOBUTTON,12,109,68,10
CONTROL         "Every other line",IDC_RSTACK_BLEND6,"Button",BS_AUTORADIOBUTTON,12,118,65,10
CONTROL         "Subtractive 1",IDC_RSTACK_BLEND5,"Button",BS_AUTORADIOBUTTON,12,128,58,10
CONTROL         "Subtractive 2",IDC_RSTACK_BLEND10,"Button",BS_AUTORADIOBUTTON,12,138,58,10
CONTROL         "Xor",IDC_RSTACK_BLEND7,"Button",BS_AUTORADIOBUTTON,12,148,27,10
CONTROL         "Maximum",IDC_RSTACK_BLEND8,"Button",BS_AUTORADIOBUTTON,12,158,45,10
CONTROL         "Minimum",IDC_RSTACK_BLEND9,"Button",BS_AUTORADIOBUTTON,12,168,43,10
CONTROL         "Multiply",IDC_RSTACK_BLEND11,"Button",BS_AUTORADIOBUTTON,12,178,39,10
CONTROL         "Adjustable:",IDC_RSTACK_BLEND12,"Button",BS_AUTORADIOBUTTON,12,188,51,10
	*/

	constexpr eastl::array<eBlendMode, 12> modes =
	{
		eBlendMode::Replace, eBlendMode::Fifty, eBlendMode::Additive, eBlendMode::PixelInterleave,
		eBlendMode::Subtractive1, eBlendMode::LineInterleave, eBlendMode::Xor, eBlendMode::Maximum,
		eBlendMode::Minimum, eBlendMode::Subtractive, eBlendMode::Multiply, eBlendMode::Adjustable
	};
	const eBlendMode mode = ( avs.blend >= 0 && avs.blend < (int)modes.size() ) ? modes[ avs.blend ] : eBlendMode::Replace;

	const float val = ( mode == eBlendMode::Adjustable ) ? div255 * (float)avs.adjblend_val : 0;

	return m_blend.blend( source, dest, mode, val );
}