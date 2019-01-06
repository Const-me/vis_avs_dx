#pragma once
#include "../../EffectImpl.hpp"
#include <EffectBase/ApeEffectBase.h>

class ColorMap: public ApeEffectBase
{
	__m128i m_hash;
	CComPtr<ID3D11Texture1D> m_mapTexture;
	CComPtr<ID3D11ShaderResourceView> m_mapSrv;
	Shader<Hlsl::Trans::ColorMap::ColorMapPS> m_ps;

	HRESULT updateTexture( const uint32_t* pData );

public:
	ColorMap( C_RBASE* pThis );

	DECLARE_EFFECT()

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};