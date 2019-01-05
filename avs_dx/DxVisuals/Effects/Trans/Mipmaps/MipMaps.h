#pragma once
#include <Utils/resizeHandler.h>
#include <Resources/RenderTarget.h>
#include <Effects/EffectProfiler.h>

class MipMaps : public ResizeHandler
{
	// Copy of the input frame, but with full set of mipmaps
	CComPtr<ID3D11Texture2D> m_texture;
	CComPtr<ID3D11ShaderResourceView> m_srv;

	void destroy();

	void onRenderSizeChanged() override;

	HRESULT createTextures();

public:

	HRESULT update( const RenderTarget& src );

	ID3D11ShaderResourceView* srv() const { return m_srv; }
};

class MipMapsRenderer
{
	MipMaps m_mipMaps;
#ifdef DEBUG
	CComPtr<ID3D11SamplerState> m_sampler;	// To make debug layer shut up about missing sampler
#endif
	EffectProfiler m_profileMipmaps;

protected:

	MipMapsRenderer( const char* profileName );

	// Copy the frame to mipmap texture, and re-generate the mipmaps
	HRESULT updateMipmaps( RenderTarget& src );

	// Bind mipmaps and the sampler to the PS stage
	HRESULT bindMipmaps( UINT samplerSlot );
};