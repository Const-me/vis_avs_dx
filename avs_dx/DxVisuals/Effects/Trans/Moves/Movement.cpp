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
	tBase( pState ),
	m_ps( eastl::in_place<DynamicShader> )
{ }

template<class S>
HRESULT Movement::updateShader( S& ps, Binder& binder )
{
	BoolHr hr = ps.update( binder, *avs, stateData );
	if( hr.failed() )
		return hr;
	if( !hr.value() )
		return S_FALSE;
	CHECK( ps.compile( stateOffset() ) );
	return S_OK;
}

HRESULT Movement::updateParameters( Binder& binder )
{
	bool isDynamic = false;
	switch( avs->effect )
	{
	case 1:
		if( !eastl::holds_alternative<FuzzifyShader>( m_ps ) )
			m_ps.emplace<FuzzifyShader>();
		break;
	case 7:
		if( !eastl::holds_alternative<BlockyOutShader>( m_ps ) )
			m_ps.emplace<BlockyOutShader>();
		break;
	default:
		if( !eastl::holds_alternative<DynamicShader>( m_ps ) )
			m_ps.emplace<DynamicShader>();
		isDynamic = true;
		break;
	}

	if( isDynamic )
	{
		CHECK( tBase::updateParameters( binder ) );
	}

	return eastl::visit( [ & ]( auto& s ) { return updateShader( s, binder ); }, m_ps );
}

template<class S>
HRESULT Movement::renderFullscreen( S& ps, RenderTargets& rt )
{
	BoundPsResource bound;
	if( avs->blend )
	{
		CHECK( rt.blendToNext( bound ) );
		omCustomBlend( 0.5f );
	}
	else
	{
		CHECK( rt.writeToNext( bound ) );
		omBlend( eBlend::None );
	}

	if( !ps.bind() )
		return S_FALSE;
	drawFullscreenTriangle( true );
	return S_OK;
}

HRESULT Movement::render( bool isBeat, RenderTargets& rt )
{
	if( 0 == avs->effect )
		return S_FALSE;

	if( eastl::holds_alternative<DynamicShader>( m_ps ) )
	{
		if( !renderer.bindShaders( isBeat ) )
			return S_FALSE;
		DynamicShader& ps = eastl::get<DynamicShader>( m_ps );
		if( !ps.bind( isBeat ) )
			return S_FALSE;
		const UINT psSamplerSlot = ps.data().bindSampler;
		return MovementFx::render( rt, avs->subpixel, avs->wrap, psSamplerSlot, avs->blend, avs->rectangular );
	}
	return eastl::visit( [ & ]( auto& s ) { return renderFullscreen( s, rt ); }, m_ps );
}