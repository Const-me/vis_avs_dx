#include "stdafx.h"
#include "RenderWindow.h"

#pragma comment( lib, "D3D11.lib" )
CComPtr<ID3D11Device> device;
CComPtr<ID3D11DeviceContext> context;

int RenderWindow::wmCreate( LPCREATESTRUCT lpCreateStruct )
{
	destroyDevice();
	createDevice();
	return 0;
}

void RenderWindow::wmDestroy()
{
	logDebug( "WM_DESTROY" );
	destroyDevice();
}

HRESULT RenderWindow::createDevice()
{
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
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;

	CHECK( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &fl, 1, D3D11_SDK_VERSION, &scd, &m_swapChain, &device, nullptr, &context ) );

	CComPtr<ID3D11Resource> pBB;
	CHECK( m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBB ) ) );

	CHECK( device->CreateRenderTargetView( pBB, nullptr, &m_rtv ) );

	logDebug( "Created device + swap chain" );
	return S_OK;
}

void RenderWindow::destroyDevice()
{
	device = nullptr;
	context = nullptr;
	m_swapChain = nullptr;
}

HRESULT RenderWindow::wmSize( UINT nType, CSize size )
{
	logDebug( "WM_SIZE: %i, %i x %i", nType, size.cx, size.cy );

	// https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#handling-window-resizing
	if( !m_swapChain )
		return S_FALSE;

	CSLock __lock( m_deviceLock );

	context->OMSetRenderTargets( 0, nullptr, nullptr );
	m_rtv = nullptr;
	CHECK( m_swapChain->ResizeBuffers( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 ) );

	CComPtr<ID3D11Texture2D> pBuffer;
	CHECK( m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBuffer ) ) );
	CHECK( device->CreateRenderTargetView( pBuffer, nullptr, &m_rtv ) );

	CD3D11_VIEWPORT vp{ 0.0f, 0.0f, (float)size.cx, (float)size.cy };
	context->RSSetViewports( 1, &vp );

	logInfo( "Resized the swap chain to %i x %i", size.cx, size.cy );
	return S_OK;
}

LRESULT RenderWindow::wmRender( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	logDebug( "WM_RENDER" );
	return 0;
}