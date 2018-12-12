#pragma once

class StructureBuffer
{
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CComPtr<ID3D11UnorderedAccessView> m_uav;

public:
	// Create the buffer. The size is the count of 32-bit untyped elements required by all effects combined.
	HRESULT create( UINT width, UINT count );

	void destroy();

	// Read only view of the buffer
	ID3D11ShaderResourceView* srv() const { return m_srv; }

	// Read/write view of the buffer
	ID3D11UnorderedAccessView* uav() const { return m_uav; }
};