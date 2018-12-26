#pragma once
// Global interface pointers used by majority of code running on rendering thread.

extern CComPtr<ID3D11Device> device;
extern CComPtr<ID3D11DeviceContext> context;
extern CComPtr<IMFDXGIDeviceManager> dxgiDeviceManager;