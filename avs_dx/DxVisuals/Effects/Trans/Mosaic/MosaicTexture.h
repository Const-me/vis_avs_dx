#pragma once
#include <Utils/resizeHandler.h>
#include <Resources/RenderTarget.h>

class MosaicTexture: public ResizeHandler
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