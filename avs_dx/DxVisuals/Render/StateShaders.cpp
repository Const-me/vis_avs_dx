#include "stdafx.h"
#include "StateShaders.h"
#include <Hlsl/compile.h>
#include <Effects/shadersCode.h>
#include <Resources/createShader.hpp>
#include <Utils/md4.h>

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
Texture2D<float> texVisDataU8 : register(t0);
Texture2D<float> texVisDataS8 : register(t1);
SamplerState sampleBilinear : register(s0);
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
	bool anyBeat;
	const CStringA hlsl = assembleEffects( effects, anyBeat, totalStateSize );

	const __m128i hash = hashString( hlsl );
	if( isFailed() && hash == m_hash )
		return S_FALSE;
	m_hash = hash;

	dropShader();
	init = nullptr;

	std::vector<uint8_t> dxbc;
	Hlsl::Defines def;
	def.set( "INIT_STATE", "0" );

	if( hlsl.Find( "AVS_RENDER_SIZE" ) >= 0 )
	{
		def.set( "AVS_RENDER_SIZE", getRenderSizeString() );
		subscribeHandler( this );
	}
	else
		unsubscribeHandler( this );

	// Compile two main shaders
	CHECK( __super::compile( "UpdateState", hlsl, Hlsl::includes(), def, anyBeat, dxbc ) );

	// Compile state initialization shader
	def.reset( "INIT_STATE", "1" );
	if( anyBeat )
		def.reset( "IS_BEAT", "0" );

	setFailed();
	CHECK( Hlsl::compile( eStage::Compute, hlsl, "InitState", Hlsl::includes(), def, dxbc ) );
	CHECK( createShader( dxbc, init ) );
	setGood();

	logInfo( "Compiled the state shaders" );
	return S_OK;
}

StateShaders::~StateShaders()
{
	unsubscribeHandler( this );
}

void StateShaders::onRenderSizeChanged()
{
	init = nullptr;
	__super::onRenderSizeChanged();
}