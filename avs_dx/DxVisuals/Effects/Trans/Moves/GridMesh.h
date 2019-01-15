#pragma once
#include <Utils/resizeHandler.h>

class GridMesh: public ResizeHandler
{
	UINT m_indexCount;
	CComPtr<ID3D11Buffer> m_vb, m_ib;
	CSize m_cells;
	float m_triSize;
	bool m_rectangular;

	void onRenderSizeChanged() override;

	HRESULT update( bool rectangularCoords );

	// Compute ideal grid size from screen size and triangle size.
	CSize pickSize( const CSize &screen, int triangle );

	// Generate the grid mesh. It uses 16 bit indices so this will fail if the size is too large.
	HRESULT createRectangular();

	HRESULT createRadial();

	void destroy();

public:

	struct sInput
	{
		Vector2 pos, tc;
	};
	using Ind3 = array<uint32_t, 3>;

	HRESULT draw( bool rectangularCoords );
};