#pragma once

// Merged state buffer for all effects. It's merged so we have a single compute shaders updating all of them: draw calls are relatively expensive.
class EffectsState
{
	CComPtr<ID3D11ShaderResourceView> m_srv;
	CComPtr<ID3D11UnorderedAccessView> m_uav;

public:
	// Create the buffer. The size is the count of 32-bit untyped elements required by all effects combined.
	HRESULT create( UINT totalSize );

	// Read only view of the buffer
	ID3D11ShaderResourceView* srv() const { return m_srv; }

	// Read/write view of the buffer
	ID3D11UnorderedAccessView* uav() const { return m_uav; }
};