#pragma once

// Small immutable 1*x texture, with an array of colors from settings.
class ColorsTexture
{
	CComPtr<ID3D11ShaderResourceView> m_srv;

public:
	HRESULT create();
};