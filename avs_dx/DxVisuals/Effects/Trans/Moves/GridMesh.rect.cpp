#include "stdafx.h"
#include "GridMesh.h"

// Example mesh topology, for 3x2 grid
//  x---x---x---x
//  |\ / \ / \ /|
//  x-x---x---x-x
//  |/ \ / \ / \|
//  x---x---x---x

namespace
{
	using sInput = GridMesh::sInput;
	using Ind3 = GridMesh::Ind3;

	HRESULT reserveVertexBuffer( const CSize &size, vector<sInput> &vb )
	{
		const size_t vbEven = (size_t)size.cx + 1;
		const size_t vbOdd = vbEven + 1;
		const size_t nEven = (size_t)( size.cy + 2 ) / 2;
		const size_t nOdd = (size_t)( size.cy + 1 ) / 2;
		size_t totalVerts = vbEven * nEven + vbOdd * nOdd;
		vb.reserve( totalVerts );
		return S_OK;
	}

	void reserveIndexBuffer( const CSize &size, vector<Ind3> &ib )
	{
		size_t trisPerStripe = size_t( size.cx );
		trisPerStripe = trisPerStripe + trisPerStripe + 1;
		size_t totalTris = trisPerStripe * size.cy;
		ib.reserve( totalTris );
	}

	HRESULT reserveBuffers( const CSize &size, vector<sInput> &vb, vector<Ind3> &ib )
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

	// Push odd-numbered vertices row e.g. 0-th or 6-th
	void pushEvenRow( const GridDim &dim, vector<sInput> &vb, int yInt )
	{
		for( int i = 0; i <= dim.cx; i++ )
			vb.emplace_back( dim.posEven( yInt, i ) );
	}
	// Push even-numbered vertices row e.g. 1-st or 3-rd
	void pushOddRow( const GridDim &dim, vector<sInput> &vb, int yInt )
	{
		for( int i = -1; i <= dim.cx; i++ )
			vb.emplace_back( dim.posOdd( yInt, i ) );
	}

	inline void pt( vector<Ind3> &ib, uint32_t a, uint32_t b, uint32_t c )
	{
		ib.emplace_back( Ind3{ a, b, c } );
	}

	// Push triangle strip indices where bottom row has odd index.
	void pushOddStrip( const GridDim &dim, vector<Ind3> &ib, uint32_t r1, uint32_t r2 )
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
	void pushEvenStrip( const GridDim &dim, vector<Ind3> &ib, uint32_t r1, uint32_t r2 )
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
}

HRESULT GridMesh::createRectangular()
{
	destroy();
	assert( m_rectangular );

	const CSize size = m_cells;
	if( size.cx < 1 || size.cy < 1 )
		return E_INVALIDARG;

	vector<sInput> vb;
	vector<Ind3> ib;
	CHECK( reserveBuffers( size, vb, ib ) );

	const GridDim dim{ size };
	uint32_t prevRow = 0;
	pushEvenRow( dim, vb, 0 );

	for( int y = 1; y <= size.cy; y++ )
	{
		const bool odd = ( 0 != ( y & 1 ) );
		const uint32_t thisRow = (uint32_t)vb.size();
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

	{
		CD3D11_BUFFER_DESC desc{ sizeofVector( vb ), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{ vb.data(), sizeof( sInput ), 0 };
		CHECK( device->CreateBuffer( &desc, &initData, &m_vb ) );
	}

	{
		static_assert( sizeof( Ind3 ) == 12 );
		CD3D11_BUFFER_DESC desc{ sizeofVector( ib ), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{ ib.data(), 4, 0 };
		CHECK( device->CreateBuffer( &desc, &initData, &m_ib ) );
	}

	m_indexCount = ib.size() * 3;
	logDebug( "Created a rectangular grid mesh %i x %i cells", size.cx, size.cy );
	return S_OK;
}