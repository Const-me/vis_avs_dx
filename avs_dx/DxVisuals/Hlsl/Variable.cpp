#include "stdafx.h"
#include "Variable.h"

namespace
{
	using namespace Hlsl;

	struct sTypeMetadata
	{
		const uint8_t cb;
		const uint8_t widthMin, widthMax;
		const char* const hlslName;
		sTypeMetadata( uint8_t s, uint8_t i, uint8_t ax, const char* n ) :
			cb( s ), widthMin( i ), widthMax( ax ), hlslName( n ) { }
	};

	static const std::array<sTypeMetadata, (size_t)( eVarType::CountTypes )> s_metadata =
	{
		sTypeMetadata{ 4, 1, 4, "float" },
		sTypeMetadata{ 4, 1, 4, "int" },
		sTypeMetadata{ 4, 1, 4, "uint" },
		sTypeMetadata{ 4, 1, 1, "bool" },
		sTypeMetadata{ 8, 1, 1, "double" },
	};

	inline const sTypeMetadata& metadata( eVarType vt )
	{
		return s_metadata[ (uint8_t)vt ];
	}

	inline uint8_t bytesCount( eVarType vt )
	{
		return metadata( vt ).cb;
	}
}

namespace Hlsl
{
	bool Variable::validate() const
	{
		if( name[ 0 ] == '\0' )
			return false;
		const sTypeMetadata tm = s_metadata[ (uint8_t)vt ];
		if( width < tm.widthMin || width > tm.widthMax )
			return false;
		return true;
	}

	uint8_t Variable::bytesCount() const
	{
		return ::bytesCount( vt ) * width;
	}

	HRESULT appendPacked( const std::vector<Variable> &src, uint16_t effect, uint16_t &startOffset, std::vector<PackedVariable> &dest )
	{
		dest.reserve( dest.size() + src.size() );
		dest.clear();

		// It's possible to optimize by reordering variables. However, the buffer size limit is relatively high, D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT = 4096, i.e. the current version might be good enough already.
		uint16_t nextOffset = startOffset;
		uint8_t nextComponent = 0;
		for( const auto& v : src )
		{
			if( !v.validate() )
				return E_INVALIDARG;
			PackedVariable pv;
			(Variable&)pv = v;

			const uint8_t cc = pv.bytesCount() / 4;
			if( cc + nextComponent <= 4 )
			{
				// Fits in the current register
				pv.offset = nextOffset;
				pv.component = nextComponent;
				nextComponent += cc;
			}
			else
			{
				// Doesn't
				nextOffset++;
				pv.offset = nextOffset;
				pv.component = 0;
				nextComponent = cc;
			}

			dest.push_back( pv );
		}
		if( !src.empty() )
			startOffset = nextOffset;
		return S_OK;
	}

	void hlslStructure( const std::vector<PackedVariable>& vars, int nEffect, uint16_t startOffset, CStringA& hlsl )
	{
		const char* components = "xyzw";
		for( const auto& v : vars )
		{
			if( v.width == 1 )
			{
				// scalar
				const char* typeString;
				if( v.vt != eVarType::f64 )
					typeString = metadata( v.vt ).hlslName;
				else
					typeString = "uint2";
				hlsl.AppendFormat( "	%s e%i_%.8s : packoffset( c%i:%c );\r\n", typeString, nEffect, v.name.data(),
					startOffset + v.offset, components[ v.component ] );
			}
			else
			{
				// vector
				hlsl.AppendFormat( "	%s%i e%i_%.8s : packoffset( c%i:%c );\r\n", metadata( v.vt ).hlslName, nEffect, v.width, v.name.data(),
					startOffset + v.offset, components[ v.component ] );
			}
		}
	}
}