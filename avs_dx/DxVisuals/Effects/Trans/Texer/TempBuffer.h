#pragma once

class TempBuffer
{
	CComPtr<ID3D11UnorderedAccessView> m_uav;

public:

	HRESULT create();

	ID3D11UnorderedAccessView* uav() const
	{
		return m_uav;
	};
};