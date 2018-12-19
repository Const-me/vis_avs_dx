#include "stdafx.h"
#include "RenderWindow.h"
#include "../DxVisuals/Resources/staticResources.h"
#include "interop.h"
#include "../DxVisuals/Resources/RenderTarget.h"
#include <dxgi1_3.h>

#pragma comment( lib, "D3D11.lib" )
CComPtr<ID3D11Device> device;
CComPtr<ID3D11DeviceContext> context;
CComAutoCriticalSection renderLock;

constexpr UINT msPresentTimeout = 250;

constexpr uint16_t getWindowsVersion()
{
	return 0x0A00;
}

constexpr UINT buffersCount()
{
	return ( getWindowsVersion() >= _WIN32_WINNT_WINBLUE ) ? 2 : 1;
}

constexpr UINT swapChainFlags()
{
	return ( getWindowsVersion() >= _WIN32_WINNT_WINBLUE ) ? DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : 0;
}

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
	scd.BufferCount = buffersCount();
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = swapChainFlags();

	if( getWindowsVersion() >= _WIN32_WINNT_WIN10 )
	{
		scd.BufferCount = 2;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	}
	else if( getWindowsVersion() >= _WIN32_WINNT_WINBLUE )
	{
		scd.BufferCount = 2;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	}

	CHECK( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &fl, 1, D3D11_SDK_VERSION, &scd, &m_swapChain, &device, nullptr, &context ) );

	if( getWindowsVersion() >= _WIN32_WINNT_WINBLUE )
	{
		CComPtr<IDXGISwapChain2> sc2;
		CHECK( m_swapChain->QueryInterface( IID_PPV_ARGS( &sc2 ) ) );
		CHECK( sc2->SetMaximumFrameLatency( 1 ) );
	}

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
	CStringA g_renderSizeString = "float2(0,0)";
}

CSize getRenderSize()
{
	return g_renderSize;
}

const CStringA& getRenderSizeString()
{
	return g_renderSizeString;
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

	if( size.cx <= 0 || size.cy <= 0 )
	{
		logInfo( "WM_SIZE with an empty size, ignoring." );
		return S_FALSE;
	}

	CHECK( m_swapChain->ResizeBuffers( 2, 0, 0, DXGI_FORMAT_UNKNOWN, swapChainFlags() ) );

	CComPtr<ID3D11Texture2D> pBuffer;
	CHECK( m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBuffer ) ) );
	CHECK( device->CreateRenderTargetView( pBuffer, nullptr, &m_rtv ) );

	CD3D11_VIEWPORT vp{ 0.0f, 0.0f, (float)size.cx, (float)size.cy };
	context->RSSetViewports( 1, &vp );

	logInfo( "Resized the swap chain to %i x %i", size.cx, size.cy );

	g_renderSize = size;
	g_renderSizeString.Format( "float2( %i, %i )", size.cx, size.cy );

	return S_OK;
}

LRESULT RenderWindow::wmPresent( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	// No need to lock because the reason why this code runs is the rendering thread called SendMessage() API. That thread is now waiting for the result doing nothing.
	// logDebug( "WM_RENDER" );
	const RenderTarget* pSource = (const RenderTarget*)( wParam );
	HRESULT* pResult = (HRESULT*)lParam;

	if( !m_rtv )
	{
		logWarning( "Present is called while there's no render target" );
		*pResult = S_FALSE;
		return 0;
	}

	if( false )
	{
		// Debug code: just clear the RT
		const float rgba[4] = { 0,1,0,1 };
		context->ClearRenderTargetView( m_rtv, rgba );
		*pResult = m_swapChain->Present( 0, 0 );
		return 0;
	}

	setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::copyTexture );
	omSetTarget( m_rtv );
	context->OMSetBlendState( nullptr, nullptr, 0xffffffff );
	pSource->bindView( 127 );

	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	iaClearBuffers();
	context->Draw( 3, 0 );

	bindResource<eStage::Pixel>( 127 );

	*pResult = doPresent();;
	return TRUE;
}

HRESULT RenderWindow::doPresent()
{
	CHECK( m_swapChain->Present( 1, 0 ) );
	// CHECK( m_output->WaitForVBlank() );
	return S_OK;
}

HRESULT RenderWindow::sendMessageTimeout( UINT wm, const void* wParam )
{
	HRESULT hr = E_FAIL;
	DWORD_PTR dwResult = 0;
	const LRESULT res = ::SendMessageTimeout( m_hWnd, wm, (WPARAM)wParam, (LPARAM)&hr, SMTO_BLOCK | SMTO_ERRORONEXIT, msPresentTimeout, &dwResult );
	if( res != 0 )
	{
		if( SUCCEEDED( hr ) )
			return S_OK;
		return hr;
	}
	return getLastHr();
}

HRESULT RenderWindow::presentSingle( const RenderTarget& src )
{
	if( !src )
	{
		logError( "RenderWindow::presentSingle: the source is empty" );
		return E_INVALIDARG;
	}
	return sendMessageTimeout( WM_PRESENT, &src );
}

HRESULT RenderWindow::presentTransition( const RenderTarget& t1, const RenderTarget& t2, int trans, float sintrans )
{
	int flag = 0;
	if( t1 ) flag |= 1;
	if( t2 ) flag |= 2;
	switch( flag )
	{
	case 0:
		logError( "RenderWindow::presentTransition: both sources are empty" );
		return E_INVALIDARG;
	case 1:
		logWarning( "RenderWindow::presentTransition: the second source is empty" );
		return presentSingle( t1 );
	case 2:
		logWarning( "RenderWindow::presentTransition: the first source is empty" );
		return presentSingle( t2 );
	}

	sPresentTransition spt;
	spt.rt1 = &t1;
	spt.rt2 = &t2;
	spt.trans = trans;
	spt.sintrans = sintrans;

	return sendMessageTimeout( WM_TRANSITION, &spt );
}

LRESULT RenderWindow::wmTransition( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	const sPresentTransition spt = *(sPresentTransition*)( wParam );
	HRESULT* pResult = (HRESULT*)lParam;

	if( !m_rtv )
	{
		logWarning( "Present is called while there's no render target" );
		*pResult = S_FALSE;
		return 0;
	}

	// TODO: implement transitions

	*pResult = E_NOTIMPL;
	return TRUE;
}