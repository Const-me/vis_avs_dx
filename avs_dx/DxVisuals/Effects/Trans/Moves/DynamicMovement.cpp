#include "stdafx.h"
#include "DynamicMovement.h"
#include <Interop/interop.h>

IMPLEMENT_EFFECT( DynamicMovement, C_DMoveClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "b" );
			addConstantInput( "w", eVarType::u32 );	// screen width in pixels
			addConstantInput( "h", eVarType::u32 );	// screen height in pixels

			addFragmentOutput( "d" );
			addFragmentOutput( "r" );
			addFragmentOutput( "x" );
			addFragmentOutput( "y" );
			addFragmentOutput( "alpha" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

HRESULT DynamicMovementStructs::StateData::update( AvsState& avs )
{
	BoolHr hr = Compiler::update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() );
	hr.combine( updateScreenSize() );
	return hr;
}

HRESULT DynamicMovementStructs::StateData::defines( Hlsl::Defines& def ) const
{
	setExpressionMacro( def, "w", screenSize.cx );
	setExpressionMacro( def, "h", screenSize.cx );
	return S_OK;
}

HRESULT DynamicMovementStructs::VsData::updateAvs( const AvsState& avs )
{
	return CommonVsData::updateAvs( avs.rectcoords );
}

HRESULT DynamicMovementStructs::VsData::defines( Hlsl::Defines& def ) const
{
	CHECK( __super::defines( def ) );
	const CSize size = getRenderSize();
	setExpressionMacro( def, "w", size.cx );
	setExpressionMacro( def, "h", size.cx );
	return S_OK;
}

HRESULT DynamicMovementStructs::PsData::updateAvs( const AvsState& avs )
{
	return updateValue( blending, (bool)avs.blend );
}

DynamicMovementStructs::StateData::StateData( AvsState& ass ) :
	CommonStateData( "DynamicMovement", prototype() )
{ }

DynamicMovement::DynamicMovement( AvsState *pState ) :
	tBase( pState )
{ }

HRESULT DynamicMovement::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	const UINT psSamplerSlot = renderer.pixel().bindSampler;
	return MovementFx::render( rt, avs->subpixel, avs->wrap, psSamplerSlot, avs->blend ? eMovementBlend::PerVertex : eMovementBlend::None, avs->rectcoords, avs->buffern );
}