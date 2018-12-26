#pragma once

class FrameTexture
{
	CComPtr<ID3D11Texture2D> m_texture;
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CRect m_rect;

public:

	operator bool() const { return nullptr != m_srv; }

	const RECT *rectangle() const
	{
		return &m_rect;
	}

	HRESULT create( const CSize& size );

	void destroy();

	// Copy last frame to the normal texture, return shader resource view of that.
	HRESULT getView( CComPtr<ID3D11ShaderResourceView>& srv );

	ID3D11Texture2D* texture() const { return m_texture; }
};