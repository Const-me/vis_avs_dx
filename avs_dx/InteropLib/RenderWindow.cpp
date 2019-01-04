#include "stdafx.h"
#include "RenderWindow.h"
#include "../DxVisuals/Resources/staticResources.h"
#include "interop.h"
#include "../DxVisuals/Resources/RenderTarget.h"
#include "deviceCreation.h"
#include "../DxVisuals/Effects/shadersCode.h"
#include "../DxVisuals/Resources/createShader.hpp"

CComAutoCriticalSection renderLock;

constexpr UINT msPresentTimeout = 250;

int RenderWindow::wmCreate( LPCREATESTRUCT lpCreateStruct )
{
	destroyDevice();
	createDevice();
	return 0;
}

void RenderWindow::wmDestroy()
{
	logShutdown( "RenderWindow::wmDestroy" );
	destroyDevice();
}

HRESULT RenderWindow::createDevice()
{
	CHECK( ::createDevice( m_hWnd ) );
	CHECK( StaticResources::create() );
	return S_OK;
}

void RenderWindow::destroyDevice()
{
	StaticResources::destroy();
	::destroyDevice();
}

namespace
{
	CSize g_renderSize;
}

extern int cfg_fs_d;

CSize getCurrentRenderSize()
{
	if( !cfg_fs_d )
		return g_renderSize;
	return CSize{ g_renderSize.cx / 2, g_renderSize.cy / 2 };
}

HRESULT RenderWindow::wmSize( UINT nType, CSize size )
{
	logDebug( "WM_SIZE: %i, %i x %i", nType, size.cx, size.cy );

	// https://docs.microsoft.com/en-us/windows/desktop/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#handling-window-resizing
	CSLock __lock( renderLock );
	CHECK( resizeSwapChain( size ) );

	g_renderSize = size;
	m_viewport = CD3D11_VIEWPORT{ 0.0f, 0.0f, (float)size.cx, (float)size.cy };
	return S_OK;
}

HRESULT RenderWindow::setupDoublingPresent()
{
	if( !StaticResources::fullScreenTriangleTC )
		CHECK( createShader( Hlsl::StaticShaders::FullScreenTriangleWithTC(), StaticResources::fullScreenTriangleTC ) );
	if( !StaticResources::copyTextureBilinear )
		CHECK( createShader( Hlsl::StaticShaders::CopyTextureBilinear(), StaticResources::copyTextureBilinear ) );

	setShaders( StaticResources::fullScreenTriangleTC, nullptr, StaticResources::copyTextureBilinear );
	bindSampler<eStage::Pixel>( 0, StaticResources::sampleBilinear );
	return S_OK;
}

LRESULT RenderWindow::wmPresent( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	// No need to lock because the reason why this code runs is the rendering thread called SendMessage() API. That thread is now waiting for the result doing nothing.
	// logDebug( "WM_RENDER" );
	const RenderTarget* pSource = (const RenderTarget*)( wParam );
	HRESULT* pResult = (HRESULT*)lParam;

	if( !renderTargetView )
	{
		logWarning( "Present is called while there's no render target" );
		const float rgba[ 4 ] = { 0, 0, 0, 0 };
		context->ClearRenderTargetView( renderTargetView, rgba );
		*pResult = swapChain->Present( 0, 0 );
		return 0;
	}

	if( false )
	{
		// Debug code: just clear the RT
		const float rgba[ 4 ] = { 0, 1, 0, 1 };
		context->ClearRenderTargetView( renderTargetView, rgba );
		*pResult = swapChain->Present( 0, 0 );
		return 0;
	}

	if( getRenderSize() == g_renderSize )
		setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::copyTexture );
	else
		setupDoublingPresent();
	context->RSSetViewports( 1, &m_viewport );
	omSetTarget( renderTargetView );
	{
		auto bound = pSource->psView( 127 );
		omBlend( eBlend::None );
		drawFullscreenTriangle( false );
	}

	*pResult = doPresent();;
	return TRUE;
}

HRESULT RenderWindow::doPresent()
{
	CHECK( swapChain->Present( 1, 0 ) );
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
	return sendMessageTimeout( WM_PRESENT, &src );
}

struct RenderWindow::sPresentTransition
{
	const RenderTarget* rt1;
	const RenderTarget* rt2;
	int trans;
	float sintrans;
};

HRESULT RenderWindow::presentTransition( const RenderTarget& t1, const RenderTarget& t2, int trans, float sintrans )
{
	int flag = 0;
	if( t1 ) flag |= 1;
	if( t2 ) flag |= 2;
	switch( flag )
	{
	case 0:
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

	if( !renderTargetView )
	{
		logWarning( "Present is called while there's no render target" );
		*pResult = S_FALSE;
		return 0;
	}

	// TODO: implement transitions

	*pResult = E_NOTIMPL;
	return TRUE;
}