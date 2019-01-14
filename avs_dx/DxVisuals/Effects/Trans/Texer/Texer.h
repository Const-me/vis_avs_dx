#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Effects/Common/ApeEffectBase.h>
#include "SpriteTexture.h"
#include "TempTexture.h"
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
	CComPtr<ID3D11Buffer> m_verts;
	CComPtr<ID3D11UnorderedAccessView> m_vertsView;

	CComPtr<ID3D11Texture2D> m_reducedTex;
	CComPtr<ID3D11UnorderedAccessView> m_reducedUav;
	CComPtr<ID3D11ShaderResourceView> m_reducedSrv;

	Shader<Hlsl::Trans::Texer::TexerReduceCS> m_reduce;
	Shader<Hlsl::Trans::Texer::TexerProduceCS> m_produce;

	TempTexture m_tmpTexture;
	TempBuffer m_tmpBuffer;

public:

	Texer( C_RBASE* pThis );

	DECLARE_EFFECT()

private:

	CSize prevSize;

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};