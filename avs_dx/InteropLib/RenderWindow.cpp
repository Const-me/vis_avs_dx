#include "stdafx.h"
#include "RenderWindow.h"
#include "../DxVisuals/Resources/staticResources.h"
#include "interop.h"
#include "../DxVisuals/Resources/RenderTarget.h"

#pragma comment( lib, "D3D11.lib" )
CComPtr<ID3D11Device> device;
CComPtr<ID3D11DeviceContext> context;
CComAutoCriticalSection renderLock;

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

	CHECK( StaticResources::create() );

	logDebug( "Created device + swap chain" );
	return S_OK;
}

void RenderWindow::destroyDevice()
{
	StaticResources::destroy();
	m_rtv = nullptr;
	m_swapChain = nullptr;
	device = nullptr;
	context = nullptr;
}

namespace
{
	CSize g_renderSize;
}

CSize getRenderSize()
{
	return g_renderSize;
}

HRESULT RenderWindow::wmSize( UINT nType, CSize size )
{
	logDebug( "WM_SIZE: %i, %i x %i", nType, size.cx, size.cy );

	// https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#handling-window-resizing
	if( !m_swapChain )
		return S_FALSE;

	CSLock __lock( renderLock );

	context->OMSetRenderTargets( 0, nullptr, nullptr );
	m_rtv = nullptr;
	CHECK( m_swapChain->ResizeBuffers( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 ) );

	CComPtr<ID3D11Texture2D> pBuffer;
	CHECK( m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBuffer ) ) );
	CHECK( device->CreateRenderTargetView( pBuffer, nullptr, &m_rtv ) );

	CD3D11_VIEWPORT vp{ 0.0f, 0.0f, (float)size.cx, (float)size.cy };
	context->RSSetViewports( 1, &vp );

	logInfo( "Resized the swap chain to %i x %i", size.cx, size.cy );

	g_renderSize = size;

	return S_OK;
}

LRESULT RenderWindow::wmRender( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	// No need to lock because the reason why this code run is the rendering thread called SendMessage() API. That thread is now waiting for the result doing nothing.
	logDebug( "WM_RENDER" );

	const RenderTarget* pSource = (const RenderTarget*)( lParam );
	HRESULT* pResult = (HRESULT*)lParam;

	setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::copyTexture );
	omSetTarget( m_rtv );
	context->OMSetBlendState( nullptr, nullptr, 0xffffffff );
	pSource->bindView( 127 );

	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	iaClearBuffers();
	context->Draw( 3, 0 );

	*pResult = m_swapChain->Present( 0, 0 );
	return 0;
}

HRESULT RenderWindow::present( const RenderTarget& src )
{
	if( !src )
	{
		logWarning( "RenderWindow::present: the source is empty" );
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;
	::SendMessage( m_hWnd, WM_RENDER, (WPARAM)&src, (LPARAM)&hr );
	return hr;
}