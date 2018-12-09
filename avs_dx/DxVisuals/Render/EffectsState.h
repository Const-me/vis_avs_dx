#pragma once

class EffectsState
{
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CComPtr<ID3D11UnorderedAccessView> m_uav;

public:
	HRESULT create( int totalSize );

	ID3D11ShaderResourceView* srv() const { return m_srv; }
	ID3D11UnorderedAccessView* uav() const { return m_uav; }
};