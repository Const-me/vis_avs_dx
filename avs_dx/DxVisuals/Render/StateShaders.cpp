#include "stdafx.h"
#include "StateShaders.h"
#include "../Hlsl/compile.h"
#include "../Effects/shadersCode.h"
#include "../Resources/createShader.hpp"

namespace
{
	// Un-duplicate global pieces but preserve their order, they may depend upon each other.
	class GlobalPieces
	{
		std::vector<CStringA> list;
		CAtlMap<CStringA, bool> map;
		int totalLength = 0;

	public:
		void add( const std::vector<CStringA> *pGlobals )
		{
			if( nullptr == pGlobals )
				return;
			for( const auto& s : *pGlobals )
			{
				if( nullptr != map.Lookup( s ) )
					continue;
				map[ s ] = true;
				list.push_back( s );
				totalLength += s.GetLength() + 2;
			}
		}

		void write( CStringA& hlsl ) const
		{
			hlsl.Preallocate( hlsl.GetLength() + totalLength );
			for( const auto& s : list )
			{
				hlsl += s;
				hlsl += "\r\n";
			}
		}
	};

	CStringA assembleEffects( const std::vector<EffectStateShader> &effects, bool &anyBeat, UINT& totalStateSize )
	{
		anyBeat = false;
		bool needRng = false;
		GlobalPieces globals;
		for( const auto &e : effects )
		{
			if( nullptr == e.shaderTemplate )
				continue;

			anyBeat = anyBeat || e.shaderTemplate->hasBeat;
			needRng = needRng || e.shaderTemplate->hasRandomNumbers;
			globals.add( e.shaderTemplate->globals );
		}

		CStringA hlsl = R"fffuuu(
#include "FrameGlobalData.fx"
RWByteAddressBuffer effectStates : register(u0);
)fffuuu";

		globals.write( hlsl );

		hlsl += R"fffuuu(
[numthreads( 1, 1, 1 )]
void main()
{
)fffuuu";

		if( needRng )
		{
			hlsl += R"fffuuu(	uint rng_state = avs_rand_init( getTickCount );
)fffuuu";
		}

		UINT stateOffset = 0;
		CStringA offsetString;
		for( const auto& e : effects )
		{
			if( e.shaderTemplate )
			{
				Hlsl::Defines vals = e.values;
				vals.set( "STATE_OFFSET", stateOffset * 4 );
				CStringA main = vals.expand( e.shaderTemplate->hlslMain );

				hlsl += main;
				hlsl += "\r\n";
			}
			stateOffset += e.stateSize;
		}

		hlsl += "}";
		totalStateSize = stateOffset;
		return hlsl;
	}
}

HRESULT StateShaders::compile( const std::vector<EffectStateShader> &effects, UINT& totalStateSize )
{
	init = update = updateOnBeat = nullptr;
	bool anyBeat;
	const CStringA hlsl = assembleEffects( effects, anyBeat, totalStateSize );

	std::vector<uint8_t> dxbc;
	Hlsl::Defines def;
	if( anyBeat ) def.set( "IS_BEAT", "false" );

	CHECK( Hlsl::compile( eStage::Compute, hlsl, "UpdateState", Hlsl::includes(), def, dxbc ) );

	CHECK( createShader( dxbc, update ) );

	if( anyBeat )
	{
		def.clear();
		def.set( "IS_BEAT", "true" );

		CHECK( Hlsl::compile( eStage::Compute, hlsl, "UpdateStateOnBeat", Hlsl::includes(), def, dxbc ) );

		CHECK( createShader( dxbc, updateOnBeat ) );
	}
	else
		updateOnBeat = update;

	def.clear();
	def.set( "INIT_STATE", "true" );
	if( anyBeat ) def.set( "IS_BEAT", "false" );

	if( hlsl.Find( "AVS_RENDER_SIZE" ) >= 0 )
	{
		def.set( "AVS_RENDER_SIZE", getRenderSizeString() );
		subscribeHandler( this );
	}
	else
		unsubscribeHandler( this );

	CHECK( Hlsl::compile( eStage::Compute, hlsl, "InitState", Hlsl::includes(), def, dxbc ) );

	CHECK( createShader( dxbc, init ) );

	logInfo( "Compiled the state shaders" );
	return S_OK;
}

StateShaders::~StateShaders()
{
	unsubscribeHandler( this );
}

void StateShaders::onRenderSizeChanged()
{
	init = update = updateOnBeat = nullptr;
}