#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Effects/Common/ApeEffectBase.h>
#include "SpriteTexture.h"
#include "TempBuffer.h"

struct TexerStructs
{
	using AvsState = C_RBASE;

	struct StateData : public Hlsl::Trans::Texer::TexerState
	{
		StateData( const AvsState& s ) { }
		static inline UINT stateSize() { return 4; }
		HRESULT defines( Hlsl::Defines& def ) const
		{
			return S_FALSE;
		}
		HRESULT update( const C_RBASE & nativeFx );

		SpriteTexture m_sprite;
		uint32_t m_nParticles = 0;
	};

	using CsData = Hlsl::Trans::Texer::LocalMaximaCS;

	struct VsData : Hlsl::Trans::Texer::TexerVS
	{
		HRESULT updateDx( const StateData& sd );
	};

	struct GsData : public Hlsl::Trans::Texer::TexerGS
	{
		HRESULT updateDx( const StateData& sd );
	};

	using PsData = Hlsl::Trans::Texer::TexerPS;
};

class Texer : public EffectBase1<TexerStructs, ApeEffectBase>
{
	TempBuffer m_tmpBuffer;

public:

	Texer( C_RBASE* pThis );

	DECLARE_EFFECT()

private:

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};