#include "stdafx.h"
#include "GridMesh.h"
#include <Resources/staticResources.h>

constexpr int triangleIdealSizePx = 11;

CSize GridMesh::pickSize( const CSize &screen, int triangle )
{
	CSize res;
	const double triInv = 1.0 / triangle;
	res.cx = lround( triInv * screen.cx );

	const double heightInv = triInv * ( 1.0 / 0.86602540378 );	// sin ( 60 deg )
	res.cy = lround( heightInv * screen.cy );

	if( !m_rectangular )
		res.cy |= 1;	// Make Y odd so we don't have any vertex exactly in the center, only triangle.
	return res;
}

HRESULT GridMesh::draw( bool rectangularCoords )
{
	CHECK( update( rectangularCoords ) );

	if( nullptr == m_vb || nullptr == m_ib || nullptr == StaticResources::layoutPos2Tc2 )
		return E_POINTER;

	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->IASetInputLayout( StaticResources::layoutPos2Tc2 );
	iaSetBuffer( m_vb, sizeof( sInput ), m_ib, DXGI_FORMAT_R32_UINT );
	context->DrawIndexed( m_indexCount, 0, 0 );
	return S_OK;
}

CSize getRenderSize();

void GridMesh::onRenderSizeChanged()
{
	const CSize pixels = getRenderSize();
	const CSize cells = pickSize( pixels, 32 );
	if( cells == m_cells )
		return;
	destroy();
}

void GridMesh::destroy()
{
	m_vb = nullptr;
	m_ib = nullptr;
}

HRESULT GridMesh::update( bool rectangularCoords )
{
	if( m_vb && m_ib && m_rectangular == rectangularCoords )
		return S_FALSE;
	m_rectangular = rectangularCoords;
	const CSize pixels = getRenderSize();
	m_cells = pickSize( pixels, triangleIdealSizePx );
	if( m_rectangular )
		CHECK( createRectangular() )
	else
		CHECK( createRadial() )
	return S_OK;
}