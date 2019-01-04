#include "stdafx.h"
#include "device.h"
#include <dxgi1_3.h>
#include <mfapi.h>
#include <d3d11_4.h>

#pragma comment( lib, "D3D11.lib" )
CComPtr<ID3D11Device> device;
CComPtr<ID3D11DeviceContext> context;
CComPtr<IMFDXGIDeviceManager> dxgiDeviceManager;
CComPtr<IDXGISwapChain> swapChain;
CComPtr<ID3D11RenderTargetView> renderTargetView;
#ifdef DEBUG
CComPtr<ID3D11Debug> deviceDebug;
#endif

namespace
{
	UINT buffersCount()
	{
		return ( IsWindows8Point1OrGreater() ) ? 2 : 1;
	}

	UINT swapChainFlags()
	{
		return ( IsWindows8Point1OrGreater() ) ? DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : 0;
	}

	HRESULT initVideo()
	{
		UINT resetToken = 0;
		CHECK( MFCreateDXGIDeviceManager( &resetToken, &dxgiDeviceManager ) );

		CHECK( dxgiDeviceManager->ResetDevice( device, resetToken ) );

		CComQIPtr<ID3D10Multithread> mt = context.operator ->();
		if( mt )
			mt->SetMultithreadProtected( TRUE );

		return S_OK;
	}
}

HRESULT createDevice( HWND hWndOutput )
{
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#ifdef DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = buffersCount();
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWndOutput;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = swapChainFlags();

	if( IsWindows10OrGreater() )
	{
		scd.BufferCount = 2;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	}
	else if( IsWindows8Point1OrGreater() )
	{
		scd.BufferCount = 2;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	}

	if( IsWindows8OrGreater() )
	{
		bool haveVideo = true;
		if( FAILED( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &fl, 1, D3D11_SDK_VERSION, &scd, &swapChain, &device, nullptr, &context ) ) )
		{
			haveVideo = false;
			deviceFlags &= ~D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
			CHECK( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &fl, 1, D3D11_SDK_VERSION, &scd, &swapChain, &device, nullptr, &context ) );
		}

		if( haveVideo )
		{
			if( FAILED( initVideo() ) )
			{
				dxgiDeviceManager = nullptr;
				haveVideo = false;
			}
		}

		if( !haveVideo )
			logWarning( "Your GPU driver doesn't report support for hardware video decoding. Videos will decode on CPU." );
	}
	else
	{
		deviceFlags &= ~D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
		CHECK( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &fl, 1, D3D11_SDK_VERSION, &scd, &swapChain, &device, nullptr, &context ) );
	}

#ifdef DEBUG
	CHECK( device->QueryInterface( IID_PPV_ARGS( &deviceDebug ) ) );
#endif

	if( IsWindows8Point1OrGreater() )
	{
		CComPtr<IDXGISwapChain2> sc2;
		CHECK( swapChain->QueryInterface( IID_PPV_ARGS( &sc2 ) ) );
		CHECK( sc2->SetMaximumFrameLatency( 1 ) );
	}

	CComPtr<ID3D11Resource> pBB;
	CHECK( swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBB ) ) );

	CHECK( device->CreateRenderTargetView( pBB, nullptr, &renderTargetView ) );

	logDebug( "Created device + swap chain" );
	return S_OK;
}

void destroyDevice()
{
	logShutdown( "destroyDevice" );
	renderTargetView = nullptr;
	swapChain = nullptr;
	dxgiDeviceManager = nullptr;

	dxgiDeviceManager = nullptr;
	context = nullptr;

#ifdef DEBUG
	if( deviceDebug )
	{
		// __debugbreak();
		deviceDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY );
		deviceDebug = nullptr;
	}
#endif

	device = nullptr;
}

HRESULT resizeSwapChain( const CSize& size )
{
	if( !swapChain )
		return S_FALSE;

	context->OMSetRenderTargets( 0, nullptr, nullptr );
	renderTargetView = nullptr;

	if( size.cx <= 0 || size.cy <= 0 )
	{
		logInfo( "WM_SIZE with an empty size, ignoring." );
		return S_FALSE;
	}

	CHECK( swapChain->ResizeBuffers( 2, 0, 0, DXGI_FORMAT_UNKNOWN, swapChainFlags() ) );

	CComPtr<ID3D11Texture2D> pBuffer;
	CHECK( swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBuffer ) ) );
	CHECK( device->CreateRenderTargetView( pBuffer, nullptr, &renderTargetView ) );

	logInfo( "Resized the swap chain to %i x %i", size.cx, size.cy );

	return S_OK;
}