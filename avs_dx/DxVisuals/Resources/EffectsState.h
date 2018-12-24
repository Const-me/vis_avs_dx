#pragma once

// Merged state buffer for all effects. It's merged so we have a single compute shaders updating all of them: draw calls are relatively expensive.
class EffectsState
{
	CComPtr<ID3D11Buffer> m_buffer;
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CComPtr<ID3D11UnorderedAccessView> m_uav;
	UINT m_size = 0;

public:
	// Create the buffer. The size is the count of 32-bit untyped elements required by all effects combined.
	HRESULT create( UINT totalSize );

	UINT getSize() const { return m_size; }

	void destroy();

	// Read only view of the buffer
	ID3D11ShaderResourceView* srv() const { return m_srv; }

	// Read/write view of the buffer
	ID3D11UnorderedAccessView* uav() const { return m_uav; }

	ID3D11Buffer* buffer() const { return m_buffer; }
};