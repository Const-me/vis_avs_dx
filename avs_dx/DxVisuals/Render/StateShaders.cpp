#include "stdafx.h"
#include "StateShaders.h"
#include <Hlsl/compile.h>
#include <Effects/shadersCode.h>
#include <Resources/createShader.hpp>
#include <Utils/md4.h>
#include "setSizeDefines.h"

namespace
{
	// Un-duplicate global pieces but preserve their order, they may depend upon each other.
	class GlobalPieces
	{
		vector<CStringA> list;
		CAtlMap<CStringA, bool> map;
		int totalLength = 0;

	public:
		void add( const vector<CStringA> *pGlobals )
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

	CStringA assembleEffects( const vector<EffectStateShader> &effects, bool &anyBeat, UINT& totalStateSize )
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
#include "FrameGlobalData.hlsli"
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

	template<bool init, bool usesBeat, bool onBeat>
	static void setStateMacros( Hlsl::Defines& def )
	{
		if( init )
		{
			def.set( "INIT_STATE", "1" );
			if( usesBeat )
				def.set( "IS_BEAT", "0" );
			return;
		}

		def.set( "INIT_STATE", "0" );
		if( usesBeat )
			def.set( "IS_BEAT", onBeat ? "1" : "0" );
	}
}

StateShaders::StateShaders() :
	m_initCompiler( eStage::Compute, Hlsl::includes() ),
	m_updateCompiler( eStage::Compute, Hlsl::includes() )
{ }

StateShaders::~StateShaders()
{
	m_updateCompiler.shutdownWorker();
	unsubscribeHandler( this );
}

StateShaders::operator bool() const
{
	return m_init && m_update && m_updateOnBeat;
}

using namespace Hlsl;

HRESULT StateShaders::compile( const vector<EffectStateShader> &effects, UINT& totalStateSize )
{
	bool anyBeat;
	const CStringA hlsl = assembleEffects( effects, anyBeat, totalStateSize );

	const __m128i hash = hashString( hlsl );
	if( m_failed && hash == m_hash )
		return S_FALSE;
	m_hash = hash;

	dropShaders();
	Defines def;
	setSizeDefines( hlsl, def, this );

	if( anyBeat )
	{
		const Job init{ "InitState", &m_init.p, &setStateMacros<true, true, false> };
		m_initCompiler.submit( hlsl, def, &init, 1 );

		const Job update[ 2 ] =
		{
			Job{ "UpdateState", &m_update.p, &setStateMacros<false, true, false> },
			Job{ "UpdateStateBeat", &m_updateOnBeat.p, &setStateMacros<false, true, true> },
		};
		m_updateCompiler.submit( hlsl, def, update, 2 );
	}
	else
	{
		const Job init{ "InitState", &m_init.p, &setStateMacros<true, false, false> };
		m_initCompiler.submit( hlsl, def, &init, 1 );

		const Job update{ "UpdateState", &m_update.p, &m_updateOnBeat.p, &setStateMacros<false, false, false> };
		m_updateCompiler.submit( hlsl, def, &update, 1 );
	}

	m_pending = true;
	m_initCompiler.join();
	const eAsyncStatus as = m_initCompiler.asyncStatus();
	if( as == eAsyncStatus::Failed )
	{
		m_failed = true;
		return E_FAIL;
	}

	m_failed = false;
	logInfo( "Compiled the state init shader" );
	return S_OK;
}

HRESULT StateShaders::bindInitShader()
{
	if( !m_init )
		return E_POINTER;
	bindShader<eStage::Compute>( m_init );
	return S_OK;
}

HRESULT StateShaders::bindUpdateShader( bool isbeat )
{
	if( m_failed )
		return E_FAIL;

	if( m_pending )
	{
		m_updateCompiler.join();
		const eAsyncStatus as = m_updateCompiler.asyncStatus();
		if( as == eAsyncStatus::Failed )
		{
			m_failed = true;
			return E_FAIL;
		}
		m_pending = false;
	}

	bindShader<eStage::Compute>( isbeat ? m_updateOnBeat : m_update );
	return S_OK;
}

void StateShaders::onRenderSizeChanged()
{
	dropShaders();
}

void StateShaders::dropShaders()
{
	m_init = m_update = m_updateOnBeat = nullptr;
}