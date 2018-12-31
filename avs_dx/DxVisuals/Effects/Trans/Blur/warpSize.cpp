#include "stdafx.h"
#include "warpSize.h"

namespace
{
	HRESULT detectWarpSize( uint32_t &result )
	{
		CComQIPtr<IDXGIDevice> dxgiDev = device.operator ->();
		if( !dxgiDev )
			return E_NOINTERFACE;

		CComPtr<IDXGIAdapter> adapter;
		CHECK( dxgiDev->GetAdapter( &adapter ) );

		DXGI_ADAPTER_DESC desc;
		CHECK( adapter->GetDesc( &desc ) );

		switch( desc.VendorId )
		{
		case 0x8086:	// Intel
			result = 32;
			return S_OK;
		case 0x10DE:	// nVidia
			result = 32;
			return S_OK;
		case 0x1002:	// AMD
			result = 64;
			return S_OK;
		}
		logWarning( "Unexpected GPU vendor %04X (device ID %04X)", (int)desc.VendorId, (int)desc.DeviceId );
		return E_FAIL;
	}

	class WarpSize
	{
		uint32_t m_size;

	public:
		WarpSize()
		{
			if( SUCCEEDED( detectWarpSize( m_size ) ) )
				return;
			m_size = 32;
		}

		uint32_t result() const 
		{
			return m_size; 
		}
	};
}

uint32_t getWarpSize()
{
	static const WarpSize s_ws;
	return s_ws.result();
}