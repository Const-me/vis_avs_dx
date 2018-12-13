#pragma once
#include "../EffectBase/EffectBase.h"
#include "../Render/EffectRenderer.hpp"

template<class TStruct>
class EffectBase1 : public TStruct, public EffectBase
{
public:
	using tBase = EffectBase1<TStruct>;
	using AvsState = typename TStruct::AvsState;

protected:

	typename TStruct::AvsState* const avs;
	EffectBase1( typename TStruct::AvsState* ass ) : avs( ass ), m_stateData( *ass ) { }

	void bindShaders()
	{
		m_render.bindShaders();
	}

private:

	EffectRenderer<TStruct> m_render;
	typename TStruct::StateData m_stateData;

	HRESULT shouldRebuildState() override
	{
		__if_exists( TStruct::StateData::update )
		{
			return m_stateData.update( *avs );
		}
		__if_not_exists( TStruct::StateData::update )
		{
			const TStruct::StateData newState{ *avs };
			return m_stateData == newState ? S_FALSE : S_OK;
		}
		return E_FAIL;
	}

	HRESULT buildState( EffectStateShader& ess ) override
	{
		ess.shaderTemplate = m_stateData.shaderTemplate();
		ess.stateSize = m_stateData.stateSize();
		return m_stateData.defines( ess.values );
	}

	HRESULT updateParameters( Binder& binder ) override
	{
		const bool changedBindings = m_render.updateBindings( binder );
		const HRESULT hr = m_render.updateValues( *avs, stateOffset() );;
		CHECK( hr );
		const bool changedData = ( hr != S_FALSE );
		const bool changedSomething = changedBindings || changedData;
		if( !changedSomething )
			return S_FALSE;

		const CAtlMap<CStringA, CStringA>* pIncludes = &::Hlsl::includes();
		__if_exists( TStruct::effectIncludes )
		{
			pIncludes = &TStruct::effectIncludes();
		}

		CHECK( m_render.compileShaders( *pIncludes ) );
		return S_OK;
	}
};

struct EmptyStateData
{
	template<class A>
	EmptyStateData( const A& ) { }
	const StateShaderTemplate* shaderTemplate() { return nullptr; }
	UINT stateSize() { return 0; }
	HRESULT defines( Hlsl::Defines& def ) const { return S_FALSE; }
	bool operator==( const EmptyStateData& ) { return true; }
};