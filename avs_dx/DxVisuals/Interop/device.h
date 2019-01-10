#pragma once

// Global interface pointers used by majority of code running on rendering thread.
extern CComPtr<ID3D11Device> device;
extern CComPtr<ID3D11DeviceContext> context;

// This one is only used by video effects.
extern CComPtr<IMFDXGIDeviceManager> dxgiDeviceManager;

// Size of D3D window in pixels.
CSize getCurrentRenderSize();

D3D_FEATURE_LEVEL getFeatureLevel();