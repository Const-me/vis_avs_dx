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
	EffectBase1( typename TStruct::AvsState* ass ) : avs( ass ), stateData( *ass ) { }

	EffectRenderer<TStruct> renderer;

	typename TStruct::StateData stateData;

	HRESULT shouldRebuildState() override
	{
		__if_exists( TStruct::StateData::update )
		{
			return stateData.update( *avs );
		}
		__if_not_exists( TStruct::StateData::update )
		{
			const TStruct::StateData newState{ *avs };
			if( stateData == newState )
				return S_FALSE;
			stateData = newState;
			return S_OK;
		}
		return E_FAIL;
	}

private:
	HRESULT buildState( EffectStateShader& ess ) override
	{
		ess.shaderTemplate = stateData.shaderTemplate();
		ess.stateSize = stateData.stateSize();
		return stateData.defines( ess.values );
	}

	HRESULT updateParameters( Binder& binder ) override
	{
		BoolHr hr = renderer.update( binder, *avs, stateData );
		if( hr.failed() )
			return hr;
		if( !hr.value() )
			return S_FALSE;

		const CAtlMap<CStringA, CStringA>* pIncludes = &::Hlsl::includes();
		__if_exists( TStruct::effectIncludes )
		{
			pIncludes = &TStruct::effectIncludes();
		}

		CHECK( renderer.compileShaders( *pIncludes, stateOffset() ) );
		return S_OK;
	}
};

struct EmptyStateData
{
	EmptyStateData() = default;
	template<class A>
	EmptyStateData( const A& ) { }
	const StateShaderTemplate* shaderTemplate() { return nullptr; }
	UINT stateSize() { return 0; }
	HRESULT defines( Hlsl::Defines& def ) const { return S_FALSE; }
	bool operator==( const EmptyStateData& ) { return true; }
};