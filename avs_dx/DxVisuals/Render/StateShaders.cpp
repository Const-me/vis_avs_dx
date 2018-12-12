#include "stdafx.h"
#include "StateShaders.h"
#include "../Hlsl/compile.h"
#include "../Effects/shadersCode.h"
#include "../Resources/createShader.hpp"

namespace
{
	CStringA assembleEffects( const std::vector<StateBuilder::Effect> &effects, bool &anyBeat )
	{
		anyBeat = false;
		CAtlMap<CStringA, bool> globals;
		for( const auto &e : effects )
		{
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

		for( const auto& e : effects )
		{
			hlsl += e.values.expand( e.shaderTemplate->hlslMain );
			hlsl += "\r\n";
		}

		hlsl += "}";
		return hlsl;
	}
}

HRESULT StateBuilder::compile( const std::vector<Effect> &effects )
{
	init = update = updateOnBeat = nullptr;
	bool anyBeat;
	const CStringA hlsl = assembleEffects( effects, anyBeat );

	std::vector<uint8_t> dxbc;
	Hlsl::Defines def;
	CHECK( Hlsl::compile( eStage::Compute, hlsl, "UpdateState", Hlsl::includes(), def, dxbc ) );

	CHECK( createShader( dxbc, update ) );

	if( anyBeat )
	{
		def.set( "IS_BEAT", "1" );

		CHECK( Hlsl::compile( eStage::Compute, hlsl, "UpdateStateOnBeat", Hlsl::includes(), def, dxbc ) );

		CHECK( createShader( dxbc, updateOnBeat ) );
	}
	else
		updateOnBeat = update;

	def.clear();
	def.set( "INIT_STATE", "1" );

	CHECK( Hlsl::compile( eStage::Compute, hlsl, "InitState", Hlsl::includes(), def, dxbc ) );

	CHECK( createShader( dxbc, init ) );

	logInfo( "Compiled the state shaders" );
	return S_OK;
}