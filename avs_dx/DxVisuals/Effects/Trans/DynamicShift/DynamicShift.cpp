#include "stdafx.h"
#include "DynamicShift.h"

IMPLEMENT_EFFECT( DynamicShift, C_ShiftClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "b" );
			addConstantInput( "w", eVarType::u32 );
			addConstantInput( "h", eVarType::u32 );

			addState( "x", 0.0f );
			addState( "y", 0.0f );
			addState( "alpha", 0.5f );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

DynamicShiftStructs::StateData::StateData( AvsState& ass ) :
	Compiler( "DynamicShift", prototype() )
{ }

HRESULT DynamicShiftStructs::StateData::update( AvsState& avs )
{
	BoolHr hr = Compiler::update( avs.effect_exp[ 0 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), "" );

	hr.combine( updateInput( screenSize, getRenderSize() ) );

	hr.combine( updateInput( blending, (bool)avs.blend ) );

	return hr;
}

HRESULT DynamicShiftStructs::StateData::defines( Hlsl::Defines& def ) const
{
	setExpressionMacro( def, "w", screenSize.cx );
	setExpressionMacro( def, "h", screenSize.cx );
	def.set( "blending", blending );
	return S_OK;
}

HRESULT DynamicShiftStructs::PsData::updateAvs( const AvsState& avs )
{
	return updateValue( blending, (bool)avs.blend );
}

DynamicShift::DynamicShift( AvsState *pState ) :
	EffectBase1( pState )
{ }

HRESULT DynamicShift::render( bool isBeat, RenderTargets& rt )
{
	CHECK( m_sampler.update( avs->subpixel, true ) );

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	const UINT psSamplerSlot = renderer.pixel().bindSampler;
	BIND_PS_SAMPLER( psSamplerSlot, m_sampler );

	drawFullscreenTriangle( false );
	return S_OK;
}