#include "stdafx.h"
#include "GridMesh.h"
#include <Expressions/variableTypes.h>
#include <Mathematics/GteIntrSegment2AlignedBox2.h>
#include <Utils/Intrinsics/sse.hpp>
#include <Utils/meshoptimizer/meshoptimizer.h>

CSize getRenderSize();

namespace
{
	constexpr float minRadiusPx = 0.5;
	constexpr float triangleMaxPx = 11;

	inline Vector2 position( float r, float a )
	{
		Vector2 dir;
		DirectX::XMScalarSinCosEst( &dir.y, &dir.x, a );
		return dir * r;
	}

	void pushVertices( vector<Vector2>& vb, float radius, uint32_t count )
	{
		vb.reserve( vb.size() + count );

		const float mul = DirectX::XM_2PI / (float)count;
		for( size_t i = 0; i < count; i++ )
			vb.push_back( position( radius, i * mul ) );
	}

	inline gte::Vector2<float> gteVec( const Vector2 &dx )
	{
		return gte::Vector2<float>{ dx.x, dx.y };
	}

	using sInput = GridMesh::sInput;
	using Ind3 = GridMesh::Ind3;

	Vector2 renderSizeFloat()
	{
		return floatSize( getRenderSize() );
	}

	class RadialConext
	{
		vector<Vector2> vb;
		vector<array<uint32_t, 3>> ib;
		const Vector2 pixelSize;

		float rPrev;
		uint32_t countPrev;

		gte::AlignedBox2<float> gteBox;

		void pushInitial( float radius );

		bool pushDoubling( float newRadius );

		bool pushSameCount( float newRadius );

		void tryPushTriangle( uint32_t a, uint32_t b, uint32_t c, bool& any );

	public:

		RadialConext() :
			pixelSize( renderSizeFloat() )
		{
			gteBox = gte::AlignedBox2<float>{ gteVec( pixelSize * -0.5f ), gteVec( pixelSize * 0.5f ) };
			pushInitial( minRadiusPx );
		}

		bool expand();

		void produceResult( vector<sInput> &resultVerts, vector<Ind3> &resultIndices );
	};

	void RadialConext::pushInitial( float radius )
	{
		assert( vb.empty() );
		pushVertices( vb, radius, 3 );

		assert( ib.empty() );
		ib.push_back( { 0, 1, 2 } );

		rPrev = radius;
		countPrev = 3;
	}

	void RadialConext::tryPushTriangle( uint32_t a, uint32_t b, uint32_t c, bool& any )
	{
		array<Vector2, 3> verts = { vb[ a ], vb[ b ], vb[ c ] };

		using namespace gte;
		FIQuery<float, Segment2<float>, AlignedBox2<float>> test;

		bool intersected = false;
		for( int i = 0; i < 3; i++ )
		{
			const auto v1 = verts[ i ];
			const auto v2 = verts[ ( i + 1 ) % 3 ];

			const Segment2<float> segment{ gteVec( v1 ), gteVec( v2 ) };
			if( test( segment, gteBox ).intersect )
			{
				intersected = true;
				break;
			}
		}
		if( !intersected )
			return;

		ib.push_back( { a, c, b } );
		any = true;
	}

	bool RadialConext::pushDoubling( float newRadius )
	{
		const uint32_t vertsBeforePush = vb.size();

		// Vertex buffer
		pushVertices( vb, newRadius, countPrev * 2 );

		// Index buffer
		bool any = false;

		const uint32_t outerStart = vertsBeforePush;
		const uint32_t innerStart = vertsBeforePush - countPrev;
		uint32_t outer = outerStart;
		uint32_t inner = innerStart;

		const uint32_t countMinusOne = countPrev - 1;

		for( uint32_t i = 0; i < countMinusOne; i++ )
		{
			tryPushTriangle( inner, outer, outer + 1, any );
			tryPushTriangle( inner, outer + 1, inner + 1, any );
			tryPushTriangle( inner + 1, outer + 1, outer + 2, any );
			inner++;
			outer += 2;
		}

		tryPushTriangle( inner, outer, outer + 1, any );
		tryPushTriangle( inner, outer + 1, innerStart, any );
		tryPushTriangle( innerStart, outer + 1, outerStart, any );

		countPrev *= 2;
		rPrev = newRadius;
		return any;
	}

	bool RadialConext::pushSameCount( float newRadius )
	{
		const uint32_t vertsBeforePush = vb.size();

		// Vertex buffer
		pushVertices( vb, newRadius, countPrev );

		// Index buffer
		bool any = false;
		const uint32_t outerStart = vertsBeforePush;
		const uint32_t innerStart = vertsBeforePush - countPrev;
		uint32_t outer = outerStart;
		uint32_t inner = innerStart;

		const uint32_t countMinusOne = countPrev - 1;
		for( uint32_t i = 0; i < countMinusOne; i++ )
		{
			tryPushTriangle( inner, outer, outer + 1, any );
			tryPushTriangle( inner, outer + 1, inner + 1, any );
			inner++;
			outer++;
		}

		tryPushTriangle( inner, outer, outerStart, any );
		tryPushTriangle( inner, outerStart, innerStart, any );
		rPrev = newRadius;
		return any;
	}

	bool RadialConext::expand()
	{
		float doubleRadius = rPrev * 3;
		float addedRadius = rPrev + triangleMaxPx;
		if( doubleRadius < addedRadius )
			return pushDoubling( doubleRadius );	// Still near the center, doubling the size

		float sizeSame = DirectX::XM_2PI * addedRadius / countPrev;
		if( sizeSame < triangleMaxPx )
			return pushSameCount( addedRadius );	// Can expand with the same count of vertices in the ring, the triangle size is still within limits

		// Need to double vertices count in the ring to stay within the limit
		return pushDoubling( addedRadius );
	}

	void RadialConext::produceResult( vector<sInput> &resultVerts, vector<Ind3> &resultIndices )
	{
		// Index = old index of vertex. Value = new, remapped index.
		vector<uint32_t> indicesRemap;
		indicesRemap.resize( vb.size(), UINT_MAX );

		// Prepare the remap vector. In the same loop, remap the indices.
		resultIndices.reserve( ib.size() );

		uint32_t nextIndex = 0;
		// Iterate triangles
		for( const auto tri : ib )
		{
			Ind3 tri16;
			// Iterate triangle's vertex
			for( int v = 0; v < 3; v++ )
			{
				const uint32_t ind32 = tri[ v ];
				uint32_t remap = indicesRemap[ ind32 ];
				if( remap == UINT_MAX )
				{
					remap = (uint32_t)nextIndex;
					indicesRemap[ ind32 ] = remap;
					nextIndex++;
				}
				tri16[ v ] = remap;
			}
			resultIndices.push_back( tri16 );
		}
		// assert( nextIndex < 0xFFFF );

		// Produce the vertex buffer
		resultVerts.resize( nextIndex );

		using namespace Intrinsics::Sse;
		const __m128 mul = set_ps( 2.0f / pixelSize.x, 2.0f / pixelSize.y, 1.0f / pixelSize.x, -1.0f / pixelSize.y );
		const __m128 add = set_ps( 0, 0, 0.5f, 0.5f );

		for( uint32_t oldInd = 0; oldInd < vb.size(); oldInd++ )
		{
			const uint32_t newInd = indicesRemap[ oldInd ];
			if( newInd == UINT_MAX )
				continue;
			sInput& newVert = resultVerts[ newInd ];
			const Vector2 pos = vb[ oldInd ];

			// Calculate all 4 values in a couple of cycles
			__m128 val = pos;
			val = movelh_ps( val, val );
			val = val * mul + add;
			storeu_ps( &newVert.pos.x, val );
		}
	}
}

HRESULT GridMesh::createRadial()
{
	destroy();
	assert( !m_rectangular );

	RadialConext context;

	int level = 1;
	while( context.expand() )
		level++;

	vector<sInput> vb;
	vector<Ind3> ib;
	context.produceResult( vb, ib );

	// Optimize the mesh. Not sure how much it helps, but even if by 0.1% it's a good idea: this mesh is only recreated on resize, i.e. 99% of time it's static and uploaded to GPU.
	const size_t index_count = ib.size() * 3;
	const size_t vertex_count = vb.size();
	uint32_t* indices = &ib[ 0 ][ 0 ];
	meshopt_optimizeVertexCache( indices, indices, index_count, vertex_count );
	meshopt_optimizeVertexFetch( vb.data(), indices, index_count, vb.data(), vertex_count, sizeof( sInput ) );

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
	logDebug( "Created a radial mesh, %i rings, %i triangles, %i vertices", level, ib.size(), vb.size() );
	return S_OK;
}