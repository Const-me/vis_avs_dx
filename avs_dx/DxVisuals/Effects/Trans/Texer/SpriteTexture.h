#pragma once

class SpriteTexture
{
	CComPtr<ID3D11ShaderResourceView> m_srv;
	Vector2 m_size;
	CStringA m_name;

public:

	HRESULT update( const char* source );

	const Vector2& size() const { return m_size; }

	ID3D11ShaderResourceView* srv() const { return m_srv; }
};