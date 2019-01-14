#include "stdafx.h"
#include "Bump.h"

IMPLEMENT_EFFECT( Bump, C_BumpClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "isbeat" );
			addState( "x", 0.5f );
			addState( "y", 0.5f );
			addState( "bi", 1.0f );
			addState( "islbeat", (uint32_t)0, true );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

BumpStructs::DynamicStateData::DynamicStateData() :
	Expressions::Compiler( "Bump", prototype(), FixedStateData::stateSize() ) { }

HRESULT BumpStructs::FixedStateData::update( const AvsState& ass )
{
	BoolHr hr;
	hr.updateValue( bumpNormal, ass.depth * div255 );
	hr.updateValue( bumpOnBeat, ass.depth2 * div255 );
	hr.updateValue( onBeat, ass.onbeat ? true : false );
	hr.updateValue( durationFrames, ass.durFrames );
	return hr;
}

HRESULT BumpStructs::StateData::update( AvsState& avs )
{
	BoolHr res = m_fixed.update( avs );
	res.combine( m_dynamic.update( avs.code3.get(), avs.code1.get(), avs.code2.get(), "" ) );
	return res;
}

UINT BumpStructs::StateData::stateSize() const
{
	return FixedStateData::stateSize() + m_dynamic.stateSize();
}

BumpStructs::StateData::StateData( const AvsState& s ) :
	m_fixed( s ),
	m_template( "SuperScope" )
{ }

const StateShaderTemplate* BumpStructs::StateData::shaderTemplate()
{
	// Merging two state shaders: one came from SuperScopeState.hlsl to cycle colors, another one from NSEEL compiler.
	const StateShaderTemplate& tFixed = *m_fixed.shaderTemplate();
	const StateShaderTemplate& tDynamic = *m_dynamic.shaderTemplate();

	if( nullptr != tFixed.globals )
		m_templateGlobals = *tFixed.globals;
	else
		m_templateGlobals.clear();

	if( nullptr != tDynamic.globals )
		for( const CStringA& s : *tDynamic.globals )
			m_templateGlobals.push_back( s );

	m_template.globals = &m_templateGlobals;

	m_template.hlslMain = tFixed.hlslMain;
	m_template.hlslMain += "\r\n";
	m_template.hlslMain += tDynamic.hlslMain;

	m_template.hasBeat = tFixed.hasBeat || tDynamic.hasBeat;
	m_template.hasRandomNumbers = tDynamic.hasRandomNumbers;

	return &m_template;
}

HRESULT BumpStructs::StateData::defines( Hlsl::Defines& def ) const
{
	CHECK( m_fixed.defines( def ) );
	CHECK( m_dynamic.defines( def ) );
	return S_OK;
}

Bump::Bump( AvsState *pState ) :
	EffectBase1( pState )
{ }

HRESULT Bump::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	if( !m_blend )
	{
		CD3D11_BLEND_DESC blendDesc{ D3D11_DEFAULT };
		D3D11_RENDER_TARGET_BLEND_DESC& rt = blendDesc.RenderTarget[ 0 ];
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_DEST_COLOR;
		rt.BlendOp = D3D11_BLEND_OP_ADD;
		rt.DestBlend = D3D11_BLEND_ZERO;
		rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
		CHECK( device->CreateBlendState( &blendDesc, &m_blend ) );
	}

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	BoundPsResource bound;
	if( avs->buffern > 0 )
	{
		bound = StaticResources::globalBuffers[ avs->buffern - 1 ].lastWritten().psView();
		CHECK( rt.writeToLast( false ) );
	}
	else
		CHECK( rt.blendToNext( bound ) );

	context->OMSetBlendState( m_blend, nullptr, UINT_MAX );
	drawFullscreenTriangle( false );
	return S_OK;
}