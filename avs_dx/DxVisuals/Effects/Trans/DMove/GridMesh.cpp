#include "stdafx.h"
#include "GridMesh.h"
#include <Resources/staticResources.h>

// Example mesh topology, for 3x2 grid:
//  x---x---x---x
//  |\ / \ / \ /|
//  x-x---x---x-x
//  |/ \ / \ / \|
//  x---x---x---x

CSize GridMesh::pickSize( const CSize &screen, int triangle )
{
	CSize res;
	const double triInv = 1.0 / triangle;
	res.cx = lround( triInv * screen.cx );

	const double heightInv = triInv * ( 1.0 / 0.86602540378 );	// sin ( 60 deg )
	res.cy = lround( heightInv * screen.cy );

	if( !m_rectangular )
		res.cy |= 1;	// Make Y odd so we don't have a center vertex, only a center triangle
	return res;
}

namespace
{
	struct sInput
	{
		Vector2 pos, tc;
	};
	using Ind3 = std::array<uint16_t, 3>;

	HRESULT reserveVertexBuffer( const CSize &size, std::vector<sInput> &vb )
	{
		const size_t vbEven = (size_t)size.cx + 1;
		const size_t vbOdd = vbEven + 1;
		const size_t nEven = (size_t)( size.cy + 2 ) / 2;
		const size_t nOdd = (size_t)( size.cy + 1 ) / 2;
		const size_t totalVerts = vbEven * nEven + vbOdd * nOdd;
		if( totalVerts > 0xFFFF )
			return E_INVALIDARG;
		vb.reserve( totalVerts );
		return S_OK;
	}

	void reserveIndexBuffer( const CSize &size, std::vector<Ind3> &ib )
	{
		size_t trisPerStripe = size_t( size.cx );
		trisPerStripe = trisPerStripe + trisPerStripe + 1;
		ib.reserve( trisPerStripe  * size.cy );
	}

	HRESULT reserveBuffers( const CSize &size, std::vector<sInput> &vb, std::vector<Ind3> &ib )
	{
		CHECK( reserveVertexBuffer( size, vb ) );
		reserveIndexBuffer( size, ib );
		return S_OK;
	}

	struct GridDim
	{
		const int cx;
		const double mulx, muly;

		GridDim( const CSize &size ) : cx( size.cx ), mulx( 1.0 / size.cx ), muly( 1.0 / size.cy ) { }

		sInput vertex( int y, double x ) const
		{
			const double yy = y * muly;

			sInput r;
			r.pos.x = (float)( x * 2 - 1 );
			r.pos.y = (float)( yy * 2 - 1 );
			r.tc.x = (float)( x );
			r.tc.y = (float)( 1 - yy );
			return r;
		}
		sInput posEven( int y, int x ) const
		{
			assert( x >= 0 && x <= cx );
			return vertex( y, x * mulx );
		}
		sInput posOdd( int y, int x ) const
		{
			assert( x >= -1 && x <= cx );
			double xx;
			if( x == -1 )
				xx = 0;
			else if( x == cx )
				xx = 1;
			else
				xx = ( x + 0.5 ) * mulx;
			return vertex( y, xx );
		}
	};

	// Push odd-numbered vertices row i.e. 0-th or 6-th
	void pushEvenRow( const GridDim &dim, std::vector<sInput> &vb, int yInt )
	{
		for( int i = 0; i <= dim.cx; i++ )
			vb.emplace_back( dim.posEven( yInt, i ) );
	}
	// Push even-numbered vertices row i.e. 1-st or 3-rd
	void pushOddRow( const GridDim &dim, std::vector<sInput> &vb, int yInt )
	{
		for( int i = -1; i <= dim.cx; i++ )
			vb.emplace_back( dim.posOdd( yInt, i ) );
	}

	inline void pt( std::vector<Ind3> &ib, uint16_t a, uint16_t b, uint16_t c )
	{
		ib.emplace_back( Ind3{ a, b, c } );
	}

	// Push triangle strip indices where bottom row has odd index.
	void pushOddStrip( const GridDim &dim, std::vector<Ind3> &ib, uint16_t r1, uint16_t r2 )
	{
		for( int i = 0; i < dim.cx; i++ )
		{
			pt( ib, r2, r2 + 1, r1 );
			pt( ib, r1, r2 + 1, r1 + 1 );
			r1++;
			r2++;
		}
		pt( ib, r2, r2 + 1, r1 );
	}

	// Push triangle strip indices where bottom row has even index.
	void pushEvenStrip( const GridDim &dim, std::vector<Ind3> &ib, uint16_t r1, uint16_t r2 )
	{
		for( int i = 0; i < dim.cx; i++ )
		{
			pt( ib, r1, r2, r1 + 1 );
			pt( ib, r1 + 1, r2, r2 + 1 );
			r1++;
			r2++;
		}
		pt( ib, r1, r2, r1 + 1 );
	}

	template<class T>
	inline UINT sizeofVector( const std::vector<T>& r )
	{
		return (UINT)( r.size() * sizeof( T ) );
	}
}
#include "GridMesh.tesselate.hpp"

HRESULT GridMesh::create()
{
	destroy();

	const CSize size = m_cells;
	if( size.cx < 1 || size.cy < 1 )
		return E_INVALIDARG;

	std::vector<sInput> vb;
	std::vector<Ind3> ib;
	CHECK( reserveBuffers( size, vb, ib ) );

	const GridDim dim{ size };
	uint16_t prevRow = 0;
	pushEvenRow( dim, vb, 0 );

	for( int y = 1; y <= size.cy; y++ )
	{
		const bool odd = ( 0 != ( y & 1 ) );
		const uint16_t thisRow = (uint16_t)vb.size();
		if( odd )
		{
			pushOddRow( dim, vb, y );
			pushOddStrip( dim, ib, prevRow, thisRow );
		}
		else
		{
			pushEvenRow( dim, vb, y );
			pushEvenStrip( dim, ib, prevRow, thisRow );
		}
		prevRow = thisRow;
	}

	if( !m_rectangular )
		tesselateCenter( vb, ib );

	{
		CD3D11_BUFFER_DESC desc{ sizeofVector( vb ), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{ vb.data(), sizeof( sInput ), 0 };
		CHECK( device->CreateBuffer( &desc, &initData, &m_vb ) );
	}

	{
		static_assert( sizeof( Ind3 ) == 6 );
		CD3D11_BUFFER_DESC desc{ sizeofVector( ib ), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{ ib.data(), 2, 0 };
		CHECK( device->CreateBuffer( &desc, &initData, &m_ib ) );
	}

	m_indexCount = ib.size() * 3;
	logDebug( "Created a grid mesh %i x %i cells", size.cx, size.cy );
	return S_OK;
}

HRESULT GridMesh::draw( bool rectangularCoords )
{
	CHECK( update( rectangularCoords ) );

	if( nullptr == m_vb || nullptr == m_ib || nullptr == StaticResources::layoutPos2Tc2 )
		return E_POINTER;

	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->IASetInputLayout( StaticResources::layoutPos2Tc2 );
	iaSetBuffer( m_vb, sizeof( sInput ), m_ib, DXGI_FORMAT_R16_UINT );
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
	m_cells = pickSize( pixels, 32 );
	CHECK( create() );
	return S_OK;
}