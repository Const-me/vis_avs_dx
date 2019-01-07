#pragma once
#include "../EffectBase/EffectBase.h"
#include "../Render/EffectRenderer.hpp"
#include "../Resources/RenderTargets.h"
#include "../Resources/staticResources.h"
#include "../Resources/StructureBuffer.h"
#include "shadersCode.h"
#include "../Hlsl/Defines.h"
#include "EffectBase1.hpp"
using Hlsl::Defines;

#include "includeDefs.h"
#include "../InteropLib/effectsFactory.h"
#include "ShaderUpdatesSimple.hpp"
#include "EffectProfiler.h"
#include "../effects.h"

CSize getRenderSize();

template<class TEffect>
class EffectImpl : public TEffect
{
public:
	static const char* const s_effectName;

	EffectImpl( void* pNative ) : TEffect( ( typename TEffect::AvsState* )( pNative ) ),
		m_profiler( this )
	{ }

	~EffectImpl() override
	{
		logShutdown( s_effectName );
	}

	static inline HRESULT create( void* pState, const C_RBASE* pThis )
	{
		using tImpl = EffectImpl<TEffect>;
		unique_ptr<tImpl> res = make_unique<tImpl>( pState );
		addNewEffect( pThis, eastl::move( res ) );
		return S_OK;
	}

private:

	EffectProfiler m_profiler;

	HRESULT completedRendering() override
	{
		m_profiler.mark();
		return __super::completedRendering();
	}
};

#define DECLARE_EFFECT()	const EffectBase::Metadata& metadata() override;

#define IMPLEMENT_EFFECT( DX, NATIVE )                                         \
class NATIVE;                                                                  \
template<> HRESULT createDxEffect<NATIVE>( void* pState, const C_RBASE* pThis )\
{                                                                              \
	return EffectImpl<DX>::create( pState, pThis );                            \
};                                                                             \
const char* const EffectImpl<DX>::s_effectName = #DX;                          \
static const EffectBase::Metadata s_metadada{ EffectImpl<DX>::s_effectName, false };  \
const EffectBase::Metadata& DX::metadata(){ return s_metadada; }

template<class TEffect>
class ApeEffectImpl : public EffectImpl<TEffect>
{
public:
	ApeEffectImpl( C_RBASE* pNative ) : EffectImpl<TEffect>( pNative )
	{ }

	~ApeEffectImpl() override = default;

	static inline HRESULT create( C_RBASE* pThis )
	{
		using tImpl = ApeEffectImpl<TEffect>;
		unique_ptr<tImpl> res = make_unique<tImpl>( pThis );
		addNewEffect( pThis, eastl::move( res ) );
		return S_OK;
	}
};

#define IMPLEMENT_APE_EFFECT( DX, name, ape )                                                  \
const char* const EffectImpl<DX>::s_effectName = name;                                         \
static const ApeEffectBase::Metadata s_metadada { name, L##ape, &ApeEffectImpl<DX>::create };  \
const EffectBase::Metadata& DX::metadata(){ return s_metadada; }