#include "stdafx.h"
#include "StateShaders.h"
#include "../Hlsl/compile.h"
#include "../Effects/shadersCode.h"
#include "../Resources/createShader.hpp"

namespace
{
	CStringA assembleEffects( const std::vector<EffectStateShader> &effects, bool &anyBeat, UINT& totalStateSize )
	{
		anyBeat = false;
		CAtlMap<CStringA, bool> globals;
		for( const auto &e : effects )
		{
			if( nullptr == e.shaderTemplate )
				continue;

			anyBeat = anyBeat || e.shaderTemplate->hasBeat;
			for( const auto &s : e.shaderTemplate->globals )
				globals[ s ] = true;
		}

		CStringA hlsl = R"fffuuu(
#include "FrameGlobalData.fx"
RWByteAddressBuffer effectStates : register(u0);
)fffuuu";

		for( POSITION pos = globals.GetStartPosition(); nullptr != pos; )
		{
			const CStringA &gg = globals.GetNextKey( pos );
			hlsl += gg;
			hlsl += "\r\n";
		}

		hlsl += R"fffuuu(
[numthreads( 1, 1, 1 )]
void main()
{
)fffuuu";

		UINT stateOffset = 0;
		CStringA offsetString;
		for( const auto& e : effects )
		{
			if( e.shaderTemplate )
			{
				CStringA main = e.values.expand( e.shaderTemplate->hlslMain );
				offsetString.Format( "%i", stateOffset * 4 );
				main.Replace( "STATE_OFFSET", offsetString );

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

	CHECK( Hlsl::compile( eStage::Compute, hlsl, "InitState", Hlsl::includes(), def, dxbc ) );

	CHECK( createShader( dxbc, init ) );

	logInfo( "Compiled the state shaders" );
	return S_OK;
}