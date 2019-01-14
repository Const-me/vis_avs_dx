#pragma once

class TempBuffer
{
	CComPtr<ID3D11UnorderedAccessView> m_uav;
	CComPtr<ID3D11ShaderResourceView> m_srv;

public:

	HRESULT create();

	ID3D11UnorderedAccessView* uav() const
	{
		return m_uav;
	};

	ID3D11ShaderResourceView* srv() const
	{
		return m_srv;
	};
};