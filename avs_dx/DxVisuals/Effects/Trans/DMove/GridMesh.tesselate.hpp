#pragma once

namespace
{
	template<class T, uint8_t size>
	class DynamicArray
	{
		std::array<T, size> m_data;
		uint8_t m_count = 0;

	public:
		DynamicArray() = default;
		DynamicArray( const DynamicArray &that ) = default;
		DynamicArray( T t )
		{
			m_count = 1;
			m_data[ 0 ] = t;
		}

		void add( T v )
		{
			assert( m_count < size );
			m_data[ m_count ] = v;
			m_count++;
		}
		bool addIfNotThere( T newVal )
		{
			for( T v : *this )
				if( v == newVal )
					return false;
			add( newVal );
			return true;
		}

		decltype( auto ) begin() const
		{
			return m_data.begin();
		};
		decltype( auto ) end() const
		{
			return m_data.begin() + m_count;
		};
	};

	inline uint32_t edgeId( uint16_t v1, uint16_t v2 )
	{
		if( v1 < v2 )
			return (uint32_t)( v1 ) << 16 | v2;
		return (uint32_t)( v2 ) << 16 | v1;
	}

	inline uint16_t getOtherVertex( uint32_t e, uint16_t v )
	{
		if( ( e & 0xFFFF ) == v )
			return (uint16_t)( e >> 16 );
		assert( (uint16_t)( e >> 16 ) == v );
		return (uint16_t)( e & 0xFFFF );
	}

	using TriList = DynamicArray<uint16_t, 6>;
	// using EdgeList = DynamicArray<uint32_t, 6>;

	void buildTopology( const std::vector<Ind3> &ib, CAtlMap<uint16_t, TriList> &mapV2T )
	{
		const uint16_t size = (uint16_t)ib.size();
		for( uint16_t i = 0; i < size; i++ )
		{
			const Ind3 indices = ib[ i ];
			// vertex -> triangle map
			for( uint16_t v : indices )
			{
				auto p = mapV2T.Lookup( v );
				if( nullptr != p )
					p->m_value.add( i );
				else
					mapV2T.SetAt( v, TriList{ i } );
			}
			/*
			// vertex -> edge map
			EdgeList& e = mapE2T[ v ];
			e.addIfNotThere( edgeId( indices[ 0 ], indices[ 1 ] ) );
			e.addIfNotThere( edgeId( indices[ 0 ], indices[ 2 ] ) );
			e.addIfNotThere( edgeId( indices[ 1 ], indices[ 2 ] ) );
			*/
		}
	}

	template< typename K, typename V, class KT, class VT>
	inline const V& getAt( const CAtlMap<K, V, KT, VT> &map, typename KT::INARGTYPE key )
	{
		auto p = map.Lookup( key );
		assert( nullptr != p );
		return p->m_value;
	}

	void calculateVertexLevels( const std::vector<Ind3> &ib, const CAtlMap<uint16_t, TriList> &mapV2T, Ind3 centerTri, uint8_t lod, CAtlMap<uint16_t, uint8_t> &vertexLoD )
	{
		assert( lod > 0 );

		std::vector<uint16_t> verts;
		verts.assign( centerTri.begin(), centerTri.end() );
		for( uint16_t v : verts )
			vertexLoD[ v ] = lod;

		int e = 1;
		while( true )
		{
			lod--;
			if( lod <= 0 )
				return;
			e += 2;
			std::vector<uint16_t> verts2;
			verts2.reserve( e * 3 );
			for( uint16_t v1 : verts )
			{
				const TriList& tris = getAt( mapV2T, v1 );
				for( uint16_t t : tris )
				{
					const Ind3 triInd = ib[ t ];
					for( uint16_t v2 : triInd )
					{
						auto p = vertexLoD.Lookup( v2 );
						if( nullptr == p )
						{
							vertexLoD.SetAt( v2, lod );
							verts2.push_back( v2 );
							continue;
						}
					}
				}
			}
			verts2.swap( verts );
		}
	}

	using VertsLevels = std::array<uint8_t, 3>;
	void calculateLevels( const std::vector<Ind3> &ib, const CAtlMap<uint16_t, TriList> &mapV2T, const CAtlMap<uint16_t, uint8_t> &vertexLoD, CAtlMap<uint16_t, VertsLevels> &triangleLoD )
	{
		for( POSITION pos = vertexLoD.GetStartPosition(); pos; )
		{
			uint16_t vertex;
			uint8_t lod;
			vertexLoD.GetNextAssoc( pos, vertex, lod );
			const TriList& tris = getAt( mapV2T, vertex );
			for( uint16_t t : tris )
			{
				// Found a triangle that has at least 1 vertex in vertexLoD set.
				if( nullptr != triangleLoD.Lookup( t ) )
					continue;	// Already processed

				VertsLevels res;

				const Ind3 triangle = ib[ t ];
				for( int i = 0; i < 3; i++ )
				{
					auto p = vertexLoD.Lookup( triangle[ i ] );
					if( nullptr == p )
						res[ i ] = 0;
					else
						res[ i ] = p->m_value;
				}
				triangleLoD[ t ] = res;
			}
		}
	}

	uint16_t splitEdge( std::vector<sInput> &vb, CAtlMap<uint32_t, uint16_t> &newVerts, uint16_t v1, uint16_t v2 )
	{
		const uint32_t e = edgeId( v1, v2 );
		auto p = newVerts.Lookup( e );
		if( nullptr != p )
			return p->m_value;

		assert( vb.size() < 0xFFFF );
		const uint16_t index = (uint16_t)vb.size();

		const sInput i1 = vb[ v1 ];
		const sInput i2 = vb[ v2 ];
		sInput result;
		result.pos = ( i1.pos + i2.pos ) * 0.5f;
		result.tc = ( i1.tc + i2.tc ) * 0.5f;
		vb.push_back( result );
		newVerts[ e ] = index;
		return index;
	}

	template<class T>
	inline T min3( const std::array<T, 3> a )
	{
		return std::min( { a[ 0 ], a[ 1 ], a[ 2 ] } );
	}
	template<class T>
	inline T max3( const std::array<T, 3> a )
	{
		return std::max( { a[ 0 ], a[ 1 ], a[ 2 ] } );
	}

	struct TriContext
	{
		uint8_t minLod, maxLod;
		std::array<uint8_t, 3> edges;

		TriContext() = default;

		// Construct from the top-level triangle
		TriContext( const VertsLevels& verts )
		{
			edges[ 0 ] = std::min( verts[ 0 ], verts[ 1 ] );
			edges[ 1 ] = std::min( verts[ 1 ], verts[ 2 ] );
			edges[ 2 ] = std::min( verts[ 2 ], verts[ 0 ] );
			minLod = min3( edges );
			maxLod = max3( edges );
		}

		TriContext( uint8_t a ) :
			edges{ a, a, a }, minLod( a ), maxLod( a )
		{ }

		TriContext( uint8_t e0, uint8_t e1, uint8_t e2 ) :
			edges{ e0, e1, e2 }
		{
			minLod = min3( edges );
			maxLod = max3( edges );
		}

		void decrement()
		{
			minLod--;
			maxLod--;
			edges[ 0 ]--;
			edges[ 1 ]--;
			edges[ 2 ]--;
		}
	};

	template<uint8_t iPart>
	inline void trianglePart( const Ind3& oldIndices, const std::array<uint16_t, 3>& midpoints, const TriContext &tri, Ind3& ind, TriContext &c );

	//      v0
	//       x
	//      /0\
	// e2  x---x   e0
	//    /2\3/1\
	//   x---x---x
	// v2    e1    v1

	template<>
	inline void trianglePart<0>( const Ind3& oldIndices, const std::array<uint16_t, 3>& midpoints, const TriContext &tri, Ind3& ind, TriContext &c )
	{
		ind[ 0 ] = oldIndices[ 0 ];
		ind[ 1 ] = midpoints[ 0 ];
		ind[ 2 ] = midpoints[ 2 ];
		c = TriContext
		{
			tri.edges[ 0 ],
			tri.minLod,
			tri.edges[ 2 ],
		};
	}
	template<>
	inline void trianglePart<1>( const Ind3& oldIndices, const std::array<uint16_t, 3>& midpoints, const TriContext &tri, Ind3& ind, TriContext &c )
	{
		ind[ 0 ] = oldIndices[ 1 ];
		ind[ 1 ] = midpoints[ 1 ];
		ind[ 2 ] = midpoints[ 0 ];
		c = TriContext
		{
			tri.edges[ 1 ],
			tri.minLod,
			tri.edges[ 0 ],
		};
	}
	template<>
	inline void trianglePart<2>( const Ind3& oldIndices, const std::array<uint16_t, 3>& midpoints, const TriContext &tri, Ind3& ind, TriContext &c )
	{
		ind[ 0 ] = oldIndices[ 2 ];
		ind[ 1 ] = midpoints[ 2 ];
		ind[ 2 ] = midpoints[ 1 ];
		c = TriContext
		{
			tri.edges[ 2 ],
			tri.minLod,
			tri.edges[ 1 ],
		};
	}
	template<>
	inline void trianglePart<3>( const Ind3& oldIndices, const std::array<uint16_t, 3>& midpoints, const TriContext &tri, Ind3& ind, TriContext &c )
	{
		ind[ 0 ] = midpoints[ 0 ];
		ind[ 1 ] = midpoints[ 1 ];
		ind[ 2 ] = midpoints[ 2 ];
		c = TriContext{ tri.minLod };
	}

	inline uint16_t push( std::vector<Ind3> &ib, Ind3 nv )
	{
		assert( ib.size() < 0xFFFF );
		const uint16_t res = (uint16_t)ib.size();
		ib.push_back( nv );
		return res;
	}

	void tesselateTriangle( std::vector<sInput> &vb, std::vector<Ind3> &ib, CAtlMap<uint32_t, uint16_t> &newVertices, uint16_t id, TriContext tri );

	void splitTriangleIn4( std::vector<sInput> &vb, std::vector<Ind3> &ib, CAtlMap<uint32_t, uint16_t> &newVertices, uint16_t id, TriContext tri )
	{
		const Ind3 indices = ib[ id ];
		// All edges have non-zero LoD, split the whole triangle in 4 pieces
		tri.decrement();
		const Ind3 midpoints =
		{
			splitEdge( vb, newVertices, indices[ 0 ], indices[ 1 ] ),
			splitEdge( vb, newVertices, indices[ 1 ], indices[ 2 ] ),
			splitEdge( vb, newVertices, indices[ 2 ], indices[ 0 ] ),
		};

		TriContext nt;
		Ind3 ni;
		trianglePart<0>( indices, midpoints, tri, ni, nt );
		ib[ id ] = ni;
		tesselateTriangle( vb, ib, newVertices, id, nt );

		trianglePart<1>( indices, midpoints, tri, ni, nt );
		id = push( ib, ni );
		tesselateTriangle( vb, ib, newVertices, id, nt );

		trianglePart<2>( indices, midpoints, tri, ni, nt );
		id = push( ib, ni );
		tesselateTriangle( vb, ib, newVertices, id, nt );

		trianglePart<3>( indices, midpoints, tri, ni, nt );
		id = push( ib, ni );
		tesselateTriangle( vb, ib, newVertices, id, nt );
	}

	template<uint8_t iEdge>
	inline void splitTriangleIn2( std::vector<Ind3> &ib, uint16_t id, const Ind3& indices, const Ind3& midpoints )
	{
		constexpr uint8_t i1 = iEdge;
		constexpr uint8_t i2 = ( iEdge + 1 ) % 3;

		Ind3 nt = indices;
		nt[ i2 ] = midpoints[ i1 ];
		ib[ id ] = nt;

		nt = indices;
		nt[ i1 ] = midpoints[ i1 ];
		push( ib, nt );
	}

	void tesselateTriangle( std::vector<sInput> &vb, std::vector<Ind3> &ib, CAtlMap<uint32_t, uint16_t> &newVertices, uint16_t id, TriContext tri )
	{
		if( tri.maxLod <= 0 )
			return;

		if( tri.minLod >= 1 )
		{
			splitTriangleIn4( vb, ib, newVertices, id, tri );
			return;
		}

		// We constructed vertex levels so we should not have triangles with more than 1 difference of levels between edges.
		if( tri.maxLod != 1 )
			__debugbreak();

		const Ind3 indices = ib[ id ];
		int mask = 0;
		Ind3 midpoints{ 0xFFFF, 0xFFFF , 0xFFFF };
		for( int i = 0; i < 3; i++ )
		{
			if( 0 == tri.edges[ i ] )
				continue;
			mask |= ( 1 << i );
			midpoints[ i ] = splitEdge( vb, newVertices, indices[ i ], indices[ ( i + 1 ) % 3 ] );
		}

		//      v0
		//       x
		//      / \
		// e2  x   x   e0
		//    /     \
		//   x---x---x
		// v2    e1    v1


		switch( mask )
		{
		case 0:
			__debugbreak();
		case 0b001:	// e0 has more segments
			splitTriangleIn2<0>( ib, id, indices, midpoints );
			return;
		case 0b010:	// e1 has more segments
			splitTriangleIn2<1>( ib, id, indices, midpoints );
			return;
		case 0b100:
			splitTriangleIn2<2>( ib, id, indices, midpoints );
			return;
		}

		// For general tessellation case, need to handle other remaining 3 cases, 0b011, 0b101, 0b110, and split the triangle into 3.
		// However, this particular mesh never has them.
		__debugbreak();
	}

	void tesselate( std::vector<sInput> &vb, std::vector<Ind3> &ib, const CAtlMap<uint16_t, VertsLevels>& triLoD )
	{
		CAtlMap<uint32_t, uint16_t> newVertices;
		for( POSITION pos = triLoD.GetStartPosition(); pos; )
		{
			auto p = triLoD.GetNext( pos );
			TriContext ctx{ p->m_value };
			tesselateTriangle( vb, ib, newVertices, p->m_key, ctx );
		}
	}

	void tesselateCenter( std::vector<sInput> &vb, std::vector<Ind3> &ib )
	{
		assert( 0 != ( ib.size() & 1 ) );

		CAtlMap<uint16_t, TriList> mapV2T;
		buildTopology( ib, mapV2T );

		constexpr uint8_t centerLoD = 4;	// Subdivide center triangle edges into 2^4 = 16 segments
		CAtlMap<uint16_t, uint8_t> vertexLoD;
		calculateVertexLevels( ib, mapV2T, ib[ ib.size() / 2 ], centerLoD, vertexLoD );

		CAtlMap<uint16_t, VertsLevels> triLoD;
		calculateLevels( ib, mapV2T, vertexLoD, triLoD );

		vertexLoD.RemoveAll();
		mapV2T.RemoveAll();

		tesselate( vb, ib, triLoD );
	}
}