#include "stdafx.h"
#include "RenderWindow.h"

#pragma comment( lib, "D3D11.lib" )
CComPtr<ID3D11Device> device;
CComPtr<ID3D11DeviceContext> context;

int RenderWindow::wmCreate( LPCREATESTRUCT lpCreateStruct )
{
	device = nullptr;
	context = nullptr;

#ifdef NDEBUG
	constexpr UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#else
	constexpr UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_hWnd;
	scd.Windowed = TRUE;

	const HRESULT hr = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &fl, 1, D3D11_SDK_VERSION, &scd, nullptr, &device, nullptr, &context );
	if( SUCCEEDED( hr ) )
	{
		logDebug( "Created device + swap chain" );
		return TRUE;
	}
	logError( hr, "D3D11CreateDeviceAndSwapChain failed" );
	return FALSE;
}

void RenderWindow::wmDestroy()
{
	logDebug( "WM_DESTROY" );
	device = nullptr;
	context = nullptr;
}

void RenderWindow::wmSize( UINT nType, CSize size )
{
	logDebug( "WM_SIZE: %i, %ix%i", nType, size.cx, size.cy );
}

LRESULT RenderWindow::wmRender( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	logDebug( "WM_RENDER" );
	return 0;
}