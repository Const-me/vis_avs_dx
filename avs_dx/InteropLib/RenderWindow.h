#pragma once

class RenderWindow
{
	// The GUI thread runs a message loop deep inside winamp.exe i.e. I can't modify that in any reasonable way: binary patching, either offline or in runtime, ain't reasonable because too fragile.
	// Direct3D 11 is essentially single threaded, that's why using this workaround.
	static constexpr UINT WM_RENDER = WM_USER + 1337;

public:
	BEGIN_MSG_MAP_EX( RenderWindow )
		if( WM_NCCREATE == uMsg )
			m_hWnd = hWnd;
		MSG_WM_CREATE( wmCreate )
		MSG_WM_DESTROY( wmDestroy )
		MSG_WM_SIZE( wmSize )
		MESSAGE_HANDLER( WM_RENDER, wmRender )
	END_MSG_MAP()

private:
	HWND m_hWnd = nullptr;
	CComPtr<IDXGISwapChain> m_swapChain;
	CComPtr<ID3D11RenderTargetView> m_rtv;
	CComAutoCriticalSection m_deviceLock;

	int wmCreate( LPCREATESTRUCT lpCreateStruct );

	HRESULT createDevice();
	void destroyDevice();
	void wmDestroy();
	HRESULT wmSize( UINT nType, CSize size );
	LRESULT wmRender( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled );
};