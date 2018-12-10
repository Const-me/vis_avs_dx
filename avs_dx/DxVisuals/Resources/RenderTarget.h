#pragma once

// RGB texture with two views, read only shader view, and write-only render target view.
class RenderTarget
{
	CComPtr<ID3D11RenderTargetView> m_rtv;
	CComPtr<ID3D11ShaderResourceView> m_srv;

public:

	HRESULT create( const CSize& size );

	void clear( const Vector4& color );

	void clear()
	{
		clear( Vector4::Zero );
	}

	void bindTarget();

	void bindView( UINT slot = 0 );
};