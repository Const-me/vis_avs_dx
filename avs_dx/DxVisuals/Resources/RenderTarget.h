#pragma once

// RGB texture with two views, read only shader view, and write-only render target view.
class RenderTarget
{
	CComPtr<ID3D11Texture2D> m_tex;
	CComPtr<ID3D11RenderTargetView> m_rtv;
	CComPtr<ID3D11ShaderResourceView> m_srv;

public:

	HRESULT create();

	void destroy();

	operator bool() const
	{
		return nullptr != m_rtv && nullptr != m_srv;
	}

	void clear( const Vector4& color );

	void clear()
	{
		clear( Vector4::Zero );
	}

	// Bind the write only render target view of the texture
	void bindTarget();

	// Bind the read only shader resource view to the specified pixel shader input slot
	void bindView( UINT slot = 0 ) const;

	void copyTo( const RenderTarget& that ) const;

	void swap( RenderTarget& rt );
};