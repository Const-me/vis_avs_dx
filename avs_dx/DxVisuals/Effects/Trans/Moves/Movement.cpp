#include "stdafx.h"
#include "Movement.h"

IMPLEMENT_EFFECT( Movement, C_TransTabClass )
using namespace Expressions;

namespace
{
	class Proto : public Expressions::Prototype
	{
	public:
		Proto()
		{
			addConstantInput( "sw", eVarType::u32 );	// screen width in pixels
			addConstantInput( "sh", eVarType::u32 );	// screen height in pixels

			addFragmentOutput( "d" );
			addFragmentOutput( "r" );
			addFragmentOutput( "x" );
			addFragmentOutput( "y" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

HRESULT StaticMovementStructs::StateData::defines( Hlsl::Defines& def ) const
{
	setExpressionMacro( def, "sw", screenSize.cx );
	setExpressionMacro( def, "sh", screenSize.cy );
	return S_OK;
}

HRESULT StaticMovementStructs::VsData::defines( Hlsl::Defines& def ) const
{
	CHECK( __super::defines( def ) );

	const CSize size = getRenderSize();
	setExpressionMacro( def, "sw", size.cx );
	setExpressionMacro( def, "sh", size.cy );
	return S_OK;
}

StaticMovementStructs::StateData::StateData( AvsState& ass ) :
	CommonStateData( "Movement", prototype() )
{ }

HRESULT StaticMovementStructs::StateData::update( AvsState& avs )
{
	BoolHr hr = Compiler::update( "", "", "", avs.effect_exp.get() );
	hr.combine( updateScreenSize() );
	return hr;
}

Movement::Movement( AvsState *pState ) :
	tBase( pState )
{ }

template<class S>
HRESULT Movement::updateShader( S& ps, Binder& binder )
{
	BoolHr hr = ps.update( binder, *avs, stateData );
	if( hr.failed() )
		return hr;
	if( !hr.value() )
		return S_FALSE;
	CHECK( ps.compile( Hlsl::includes(), stateOffset() ) );
	return S_OK;
}

HRESULT Movement::updateParameters( Binder& binder )
{
	bool isDynamic = false;
	switch( avs->effect )
	{
	case 1:
		if( !std::holds_alternative<FuzzifyShader>( m_ps ) )
			m_ps.emplace<FuzzifyShader>();
		break;
	case 7:
		if( !std::holds_alternative<BlockyOutShader>( m_ps ) )
			m_ps.emplace<BlockyOutShader>();
		break;
	default:
		if( !std::holds_alternative<DynamicShader>( m_ps ) )
			m_ps.emplace<DynamicShader>();
		isDynamic = true;
		break;
	}

	if( isDynamic )
	{
		CHECK( tBase::updateParameters( binder ) );
	}

	return std::visit( [ & ]( auto& s ) { return updateShader( s, binder ); }, m_ps );
}

template<class S>
HRESULT Movement::renderFullscreen( S& ps, RenderTargets& rt )
{
	const UINT psReadSlot = ps.data().bindPrevFrame;
	BoundPsResource psRead;
	if( avs->blend )
	{
		CHECK( rt.blendToNext( psReadSlot, psRead ) );
		omCustomBlend( 0.5f );
	}
	else
	{
		CHECK( rt.writeToNext( psReadSlot, psRead, false ) );
		omBlend( eBlend::None );
	}

	setShaders( StaticResources::fullScreenTriangle, nullptr, ps.ptr( false ) );
	drawFullscreenTriangle( false );
	return S_OK;
}

HRESULT Movement::render( bool isBeat, RenderTargets& rt )
{
	if( 0 == avs->effect )
		return S_FALSE;

	if( std::holds_alternative<DynamicShader>( m_ps ) )
	{
		if( !renderer.bindShaders( isBeat ) )
			return S_FALSE;
		DynamicShader& ps = std::get<DynamicShader>( m_ps );
		if( !ps.bind( isBeat ) )
			return S_FALSE;
		const UINT psReadSlot = ps.data().bindPrevFrame;
		const UINT psSamplerSlot = ps.data().bindSampler;
		return MovementFx::render( rt, avs->subpixel, avs->wrap, psReadSlot, psSamplerSlot, avs->blend, avs->rectangular );
	}
	return std::visit( [ & ]( auto& s ) { return renderFullscreen( s, rt ); }, m_ps );
}