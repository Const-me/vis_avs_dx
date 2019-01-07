#pragma once

namespace Expressions
{
	enum eVarExtraFlag : uint16_t
	{
		None = 0,
		// The variable is fragment output, but state shader also writes to this.
		FragmentOutWrittenInState = 0x8000,
	};

	class VarUseFlags
	{
		uint16_t m_data = 0;

	public:

		uint8_t accessMask() const
		{
			return (uint8_t)( m_data & 0xFF );
		}

		void addAccessBit( eVarAccess a, uint8_t e )
		{
			assert( e < 4 );
			assert( (uint16_t)a < 4 );
			m_data |= ( (uint8_t)a ) << ( e * 2 );
		}

		void setLocation( eVarLocation loc )
		{
			assert( (uint16_t)loc < 16 );
			m_data &= 0xF0FF;
			m_data |= ( ( (uint16_t)loc ) << 8 );
		}

		eVarLocation getLocation() const
		{
			uint16_t res = ( m_data >> 8 ) & 0xF;
			return (eVarLocation)res;
		}

		void addExtraFlag( eVarExtraFlag ef )
		{
			uint16_t v = ef;
			assert( 0 == ( v & 0xFF ) );
			m_data |= v;
		}

		uint16_t extraFlags() const
		{
			return m_data & 0xF000;
		}
	};
}