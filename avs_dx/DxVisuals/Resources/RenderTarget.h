#pragma once

// RGB texture with two views, read only shader view, and write-only render target view.
class RenderTarget
{
	CComPtr<ID3D11Texture2D> m_tex;
	CComPtr<ID3D11RenderTargetView> m_rtv;
	CComPtr<ID3D11RenderTargetView> m_rtvInteger;
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CComPtr<ID3D11UnorderedAccessView> m_uav;

	ID3D11RenderTargetView* getIntegerRt();

public:

	static constexpr DXGI_FORMAT format =
		DXGI_FORMAT_R10G10B10A2_UNORM;
	// DXGI_FORMAT_R8G8B8A8_UNORM;

	HRESULT create();
	HRESULT create( const CSize& size, bool rt, bool unorderedAccess );

	void destroy();

	operator bool() const
	{
		return nullptr != m_tex;
	}

	void clear( const Vector4& color );
	void clear( const Vector3& color );
	void clear();

	// Bind the write only render target view of the texture
	void bindTarget();
	void bindTargetForLogicOp();

	void copyTo( const RenderTarget& that ) const;

	ID3D11Texture2D* texture() const { return m_tex; }

	ID3D11ShaderResourceView* srv() const { return m_srv; }

	BoundSrv<eStage::Pixel> psView() const;

	// UAVs are only required by some effects and aren't created automatically. This method creates them.
	HRESULT createUav();

	ID3D11UnorderedAccessView* uav() const { return m_uav; }
};