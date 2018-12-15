#pragma once

class GridMesh
{
	UINT m_indexCount;
	CComPtr<ID3D11Buffer> m_vb, m_ib;

public:

	// Compute ideal grid size from screen size and triangle size.
	static CSize pickSize( const CSize &screen, int triangle );

	// Generate the grid mesh. Note it uses 16 bit indices so this will fail if the size is too large.
	HRESULT create( const CSize &size );

	void destroy()
	{
		m_vb = m_ib = nullptr;
	}

	operator bool() const { return nullptr != m_vb && nullptr != m_ib; }

	HRESULT draw() const;
};