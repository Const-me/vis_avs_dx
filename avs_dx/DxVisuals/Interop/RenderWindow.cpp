#include "stdafx.h"
#include "RenderWindow.h"
#include "../DxVisuals/Resources/staticResources.h"
#include "interop.h"
#include "../DxVisuals/Resources/RenderTarget.h"
#include "deviceCreation.h"
#include "../DxVisuals/Effects/shadersCode.h"
#include "../DxVisuals/Resources/createShader.hpp"
#include "../DxVisuals/Resources/dynamicBuffers.h"
#include <algorithm> 
#include <random>
#include "profilerApi.h"
#include <Threads/AvsThreads.h>

constexpr UINT msPresentTimeout = 250;

RenderWindow* RenderWindow::s_pInstance = nullptr;

RenderWindow::RenderWindow()
{
	assert( nullptr == s_pInstance );
	s_pInstance = this;
}
RenderWindow::~RenderWindow()
{
	assert( nullptr != s_pInstance );
	s_pInstance = nullptr;
}

RenderWindow& RenderWindow::instance()
{
	assert( nullptr != s_pInstance );
	return *s_pInstance;
}

int RenderWindow::wmCreate( LPCREATESTRUCT lpCreateStruct )
{
	destroyDevice();
	createDevice();
	return 0;
}

void RenderWindow::wmDestroy()
{
	logShutdown( "RenderWindow::wmDestroy" );
	// destroyDevice();
	m_cb = nullptr;
	m_ps = nullptr;
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
	if( !StaticResources::copyTextureBilinear )
		CHECK( createShader( Hlsl::StaticShaders::CopyTextureBilinear(), StaticResources::copyTextureBilinear ) );
	setShaders( StaticResources::fullScreenTriangleTC, nullptr, StaticResources::copyTextureBilinear );
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
		auto bound = pSource->psView();
		omBlend( eBlend::None );
		drawFullscreenTriangle( false );
	}

	*pResult = doPresent();;
	return TRUE;
}

enum struct eFrameLimitMode : uint8_t;
extern eFrameLimitMode cfg_frame_limit_mode;

HRESULT RenderWindow::doPresent()
{
	HRESULT hr = swapChain->Present( 1 + (uint8_t)cfg_frame_limit_mode, 0 );
	if( SUCCEEDED( hr ) )
		return S_OK;
	if( DXGI_ERROR_DEVICE_REMOVED == hr )
	{
		HRESULT hr2 = device->GetDeviceRemovedReason();
		if( FAILED( hr2 ) )
			hr = hr2;
	}
	logError( hr, "IDXGISwapChain::Present failed" );
	// When it fails, very likely the condition's stable. No point in failing at 200Hz, 4Hz failure rate should be enough.
	Sleep( 250 );
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
	if( m_hWnd == nullptr || !IsWindow( m_hWnd ) )
	{
		/* static bool warned = false;
		if( !warned )
		{
			logWarning( "RenderWindow::presentSingle - the AVS window is destroyed" );
			warned = true;
			return S_FALSE;
		}
		Sleep( 100 );
		return S_FALSE; */
		logWarning( "RenderWindow::presentSingle - the AVS window is destroyed, shutting down everything" );
		getThreads().render().postQuitMessage();
		getThreads().gui().postQuitMessage();
		return S_FALSE;
	}

	return sendMessageTimeout( WM_PRESENT, &src );
}

struct RenderWindow::TransitionConstants
{
	float sintrans;
	uint32_t curtrans;
	uint mask;
	uint zzUnused;
};

HRESULT RenderWindow::setupTransition( int trans, float sintrans )
{
	TransitionConstants values;
	values.sintrans = sintrans;
	values.curtrans = (uint32_t)trans;
	values.mask = 0;
	if( trans == 6 && !m_inTransition )	// 9 random blocks
	{
		array<uint8_t, 9> blocks;
		for( int i = 0; i < 9; i++ )
			blocks[ i ] = i;
		// They <s>killed Kenny</s> deprecated std::random_shuffle :-(

		std::random_device rng;
		std::mt19937 urng( rng() );
		std::shuffle( blocks.begin(), blocks.end(), urng );

		uint mask = 0;
		for( int i = 0; i < 8; i++ )
		{
			mask |= 1 << blocks[ i ];
			m_randomBlocks[ i ] = mask;
		}
	}
	m_inTransition = true;

	if( trans == 6 )
	{
		int b = (int)roundf( floorf( sintrans * 8 ) );
		b = std::min( b, 7 );
		values.mask = m_randomBlocks[ b ];
	}

	values.zzUnused = 0;
	CHECK( updateCBuffer( m_cb, &values, sizeof( values ) ) );

	if( !m_ps )
		CHECK( createShader( Hlsl::StaticShaders::TransitionPS(), m_ps ) );
	setShaders( StaticResources::fullScreenTriangleTC, nullptr, m_ps );

	bindConstantBuffer<eStage::Pixel>( 0, m_cb );
	return S_OK;
}

void RenderWindow::drawTransition()
{
	context->RSSetViewports( 1, &m_viewport );
	omSetTarget( renderTargetView );
	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
}

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

	CHECK( setupTransition( trans, sintrans ) );
	omClearTargets();
	bindResource<eStage::Pixel>( 0, t1.srv() );
	bindResource<eStage::Pixel>( 1, t2.srv() );

	HRESULT hr = sendMessageTimeout( WM_TRANSITION, nullptr );

	StaticResources::sourceData.bind<eStage::Pixel>();
	return hr;
}

LRESULT RenderWindow::wmTransition( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	drawTransition();
	HRESULT* pResult = (HRESULT*)lParam;
	*pResult = doPresent();
	return TRUE;
}

LRESULT RenderWindow::wmShutdown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled )
{
	logShutdown( "RenderWindow::wmShutdown" );
	HRESULT* pResult = (HRESULT*)lParam;
	profilerClose();
	DestroyWindow(m_hWnd);
	assert( m_hWnd == nullptr );
	*pResult = S_OK;
	return TRUE;
}

bool RenderWindow::isWindow()
{
	if( nullptr == m_hWnd )
		return false;
	return ::IsWindow( m_hWnd );
}

HRESULT RenderWindow::shutdown()
{
	return sendMessageTimeout( WM_SHUTDOWN, nullptr );
}