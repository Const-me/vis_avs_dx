#pragma once

// RGB texture with two views, read only shader view, and write-only render target view.
class RenderTarget
{
	CComPtr<ID3D11Texture2D> m_tex;
	CComPtr<ID3D11RenderTargetView> m_rtv;
	CComPtr<ID3D11ShaderResourceView> m_srv;

public:

	static constexpr DXGI_FORMAT format =
		DXGI_FORMAT_R10G10B10A2_UNORM;
	// DXGI_FORMAT_R8G8B8A8_UNORM;

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

	void copyTo( const RenderTarget& that ) const;

	ID3D11Texture2D* texture() const { return m_tex; }

	ID3D11ShaderResourceView* srv() const { return m_srv; }

	BoundSrv<eStage::Pixel> psView( UINT slot ) const
	{
		return std::move( BoundSrv<eStage::Pixel>{ slot, m_srv } );
	}
};