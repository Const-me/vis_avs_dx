#pragma once

namespace
{
	// Example mesh topology, for 3x2 grid:
//  x---x---x---x
//  |\ / \ / \ /|
//  x-x---x---x-x
//  |/ \ / \ / \|
//  x---x---x---x

	CSize pickSize( const CSize &screen, int triangle )
	{
		CSize res;
		const double triInv = 1.0 / triangle;
		res.cx = lround( triInv * screen.cx );

		const double heightInv = triInv * ( 1.0 / 0.86602540378 );	// sin ( 60 deg )
		res.cy = lround( heightInv * screen.cy ) | 1;

		return res;
	}

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
}