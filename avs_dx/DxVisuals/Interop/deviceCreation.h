#pragma once
#include "device.h"

extern CComPtr<IDXGISwapChain> swapChain;
extern CComPtr<ID3D11RenderTargetView> renderTargetView;

// Create D3D 11 device, context, swap chain, render target view, and video device manager
HRESULT createDevice( HWND hWndOutput );

// Destroy all of the above
void destroyDevice();

// Resize the swap chain and render target view
HRESULT resizeSwapChain( const CSize& size );