#pragma once

class TempTexture
{
	CSize m_size;
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CComPtr<ID3D11UnorderedAccessView> m_uav;

public:

	HRESULT update();

	ID3D11ShaderResourceView* srv() const
	{
		return m_srv;
	};

	ID3D11UnorderedAccessView* uav() const
	{
		return m_uav;
	};

	CSize size() const { return m_size; }
};