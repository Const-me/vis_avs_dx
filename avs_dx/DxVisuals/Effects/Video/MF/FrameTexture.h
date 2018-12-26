#pragma once
#include <Utils/resizeHandler.h>
#include <Resources/RenderTarget.h>

class FrameTexture: public ResizeHandler
{
	CComPtr<ID3D11Texture2D> m_texture;
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CRect m_rect;

	void onRenderSizeChanged() override
	{
		destroy();
	}

public:

	static constexpr DXGI_FORMAT videoFormat = RenderTarget::format;

	operator bool() const { return nullptr != m_srv; }

	const RECT *rectangle() const
	{
		return &m_rect;
	}

	HRESULT create();

	void destroy();

	ID3D11Texture2D* texture() const { return m_texture; }
	ID3D11ShaderResourceView* view() const { return m_srv; }
};