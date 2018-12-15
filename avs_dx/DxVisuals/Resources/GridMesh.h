#pragma once

class GridMesh
{
	CComPtr<ID3D11Buffer> m_vb, m_ib;

public:

	static CSize pickSize( const CSize &screen, int triangle );
	
	HRESULT create( const CSize &size );

	void destroy()
	{
		m_vb = m_ib = nullptr;
	}
};