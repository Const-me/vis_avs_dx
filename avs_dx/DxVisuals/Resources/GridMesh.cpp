#include "stdafx.h"
#include "GridMesh.h"

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
	return res;
}

namespace
{
	struct sInput
	{
		Vector2 pos, tc;
	};
	using Ind3 = std::array<uint16_t, 3>;

	HRESULT reserveBuffer( const CSize &size, std::vector<sInput> &vb )
	{
		const size_t vbEven = (size_t)size.cx + 1;
		const size_t vbOdd = vbEven + 1;
		const size_t nEven = (size_t)( size.cy + 1 ) / 2;
		const size_t nOdd = (size_t)( size.cy + 2 ) / 2;
		const size_t totalVerts = vbEven * nEven + vbOdd * nOdd;
		if( totalVerts > 0xFFFF )
			return E_INVALIDARG;
		vb.reserve( totalVerts );
		return S_OK;
	}

	void reserveBuffer( const CSize &size, std::vector<Ind3> &ib )
	{
		size_t trisPerStripe = size_t( size.cx );
		trisPerStripe = trisPerStripe + trisPerStripe + 1;
		ib.reserve( trisPerStripe  * size.cy );
	}

	HRESULT reserveBuffers( const CSize &size, std::vector<sInput> &vb, std::vector<Ind3> &ib )
	{
		CHECK( reserveBuffer( size, vb ) );
		reserveBuffer( size, ib );
		return S_OK;
	}

	struct GridDim
	{
		const int cx;
		const double mulx, muly;

		GridDim( const CSize &size ) : cx( size.cx ), mulx( 1.0 / size.cx ), muly( 1.0 / size.cy ) { }

		sInput posEven( int y, int x ) const
		{
			sInput r;
			r.pos.x = (float)( x * mulx * 2.0 - 1.0 );
			r.pos.y = (float)( y * muly * 2.0 - 1.0 );
			r.tc.x = (float)( x * mulx );
			r.tc.y = (float)( y * muly );
			return r;
		}

		sInput posOdd( int y, int x ) const
		{
			double xx;
			if( x == -1 )
				xx = 0;
			if( x == cx )
				xx = 1;
			else
				xx = ( x + 0.5 ) * mulx;

			sInput r;
			r.pos.x = (float)( xx * 2.0 - 1.0 );
			r.pos.y = (float)( y * muly * 2.0 - 1.0 );
			r.tc.x = (float)( x * mulx );
			r.tc.y = (float)( y * muly );
			return r;
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

	// Push triangle strip where bottom row is odd.
	void pushOddStrip( const GridDim &dim, std::vector<Ind3> &ib, uint16_t r1, uint16_t r2 )
	{
		for( uint16_t i = 0; i <= dim.cx; i++ )
		{
			pt( ib, r2, r2 + 1, r1 );
			pt( ib, r1, r2 + 1, r1 + 1 );
			r1++;
			r2++;
		}
	}
	void pushEvenStrip( const GridDim &dim, std::vector<Ind3> &ib, uint16_t r1, uint16_t r2 )
	{
		for( uint16_t i = 0; i <= dim.cx; i++ )
		{
			pt( ib, r1, r2, r1 + 1 );
			pt( ib, r1 + 1, r2, r2 + 1 );
			r1++;
			r2++;
		}
	}

	template<class T>
	inline UINT sizeofVector( const std::vector<T>& r )
	{
		return (UINT)( r.size() * sizeof( T ) );
	}
}

HRESULT GridMesh::create( const CSize& size )
{
	destroy();

	if( size.cx < 1 || size.cy < 1 )
		return E_INVALIDARG;

	std::vector<sInput> vb;
	std::vector<Ind3> ib;
	CHECK( reserveBuffers( size, vb, ib ) );

	const GridDim dim{ size };
	uint16_t prevLine = 0;
	pushEvenRow( dim, vb, 0 );

	for( int i = 1; i <= size.cy; i++ )
	{
		const bool odd = ( 0 != ( i & 1 ) );
		const uint16_t ip = (uint16_t)vb.size();
		if( odd )
		{
			pushOddRow( dim, vb, i );
			pushOddStrip( dim, ib, prevLine, ip );
		}
		else
		{
			pushEvenRow( dim, vb, i );
			pushEvenStrip( dim, ib, prevLine, ip );
		}
		prevLine = ip;
	}

	{
		CD3D11_BUFFER_DESC desc{ sizeofVector( vb ), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{ vb.data(), sizeof( sInput ), 0 };
		CHECK( device->CreateBuffer( &desc, &initData, &m_vb ) );
	}

	{
		CD3D11_BUFFER_DESC desc{ sizeofVector( ib ), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{ ib.data(), 2, 0 };
		CHECK( device->CreateBuffer( &desc, &initData, &m_ib ) );
	}

	return S_OK;
}