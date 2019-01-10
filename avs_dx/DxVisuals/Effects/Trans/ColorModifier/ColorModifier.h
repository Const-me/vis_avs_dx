#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include "../../../Expressions/CompiledShader.h"

using namespace Hlsl::Trans::ColorModifier;

struct ColorModifierStructs
{
	struct AvsState
	{
		RString effect_exp[ 4 ];
		int m_recompute;
	};

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = ColorModifierPS;
	
	struct CsData : public Expressions::CompiledShader<ColorModifierCS>
	{
		HRESULT updateDx( const Expressions::Compiler& compiler )
		{
			const HRESULT hr = __super::updateDx( compiler );
			if( S_OK == hr )
				m_updated = true;
			return hr;
		}
		bool m_updated = false;
	};

	struct StateData : public Expressions::Compiler
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& avs )
		{
			return Compiler::update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() );
		}
	};
};

class ColorModifier : public EffectBase1<ColorModifierStructs>
{
	CComPtr<ID3D11Texture1D> m_texture;
	CComPtr<ID3D11UnorderedAccessView> m_uav;
	CComPtr<ID3D11ShaderResourceView> m_srv;

	HRESULT createTexture();

public:
	inline ColorModifier( AvsState *pState ) : tBase( pState ) { }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};