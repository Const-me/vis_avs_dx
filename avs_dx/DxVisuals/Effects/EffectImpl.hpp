#pragma once
#include "../EffectBase/EffectBase.h"
#include "../Render/EffectRenderer.hpp"
#include "../Resources/staticResources.h"
#include "shadersCode.h"
#include "../Hlsl/Defines.h"
#include "../Render/EffectRenderer.hpp"
using Hlsl::Defines;

// Interop with the AVS code
inline int min( int a, int b )
{
	return std::min( a, b );
}
inline int max( int a, int b )
{
	return std::max( a, b );
}
#include "../../avs/vis_avs/r_defs.h"
#include "../InteropLib/effectsFactory.h"

template<class TEffect>
class EffectImpl : public TEffect
{
public:
	EffectImpl( void* pNative ) : TEffect( ( typename TEffect::AvsState* )( pNative ) ) { }

	~EffectImpl() override = default;

	static inline HRESULT create( void* pState, std::unique_ptr<iEffect>& res )
	{
		using tImpl = EffectImpl<TEffect>;
		res = std::make_unique<tImpl>( pState );
		return S_OK;
	}
};

#define DECLARE_EFFECT( DX )                   \
const Metadata& metadata() override;

#define IMPLEMENT_EFFECT( DX, NATIVE )                                                     \
class NATIVE;                                                                              \
template<> HRESULT createDxEffect<NATIVE>( void* pState, std::unique_ptr<iEffect>& dest )  \
{                                                                                          \
	return EffectImpl<DX>::create( pState, dest );                                         \
};                                                                                         \
static const EffectBase::Metadata s_metadada{ #DX, false };                                \
const EffectBase::Metadata& DX::metadata(){ return s_metadada; }

template<class TStruct>
class EffectBase1 : public TStruct, public EffectBase
{
public:
	using tBase = EffectBase1<TStruct>;
	using AvsState = typename TStruct::AvsState;

	EffectBase1()
	{

	}

protected:

	typename TStruct::AvsState* const avs;
	EffectBase1( typename TStruct::AvsState* ass ) : avs( ass ), m_stateData( *ass ){ }

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
};

struct EmptyStateData
{
	template<class A>
	EmptyStateData( const A& ) { }
	const StateShaderTemplate* shaderTemplate() { return nullptr; }
	UINT stateSize() { return 0; }
	HRESULT defines( Defines& def ) const { return S_FALSE; }
	bool operator==( const EmptyStateData& ) { return true; }
};