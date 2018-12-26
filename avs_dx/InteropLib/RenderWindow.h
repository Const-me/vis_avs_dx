#pragma once
class RenderTarget;

class RenderWindow
{
	// The GUI thread runs a message loop deep inside winamp.exe i.e. I can't modify that in any reasonable way: binary patching, either offline or in runtime, ain't reasonable because too fragile.
	// Direct3D 11 is essentially single threaded, that's why using this workaround.
	static constexpr UINT WM_PRESENT = WM_USER + 1337;
	static constexpr UINT WM_TRANSITION = WM_PRESENT + 1;

	struct sPresentTransition
	{
		const RenderTarget* rt1;
		const RenderTarget* rt2;
		int trans;
		float sintrans;
	};

public:
	BEGIN_MSG_MAP_EX( RenderWindow )
		if( WM_NCCREATE == uMsg )
			m_hWnd = hWnd;
		MSG_WM_CREATE( wmCreate )
		MSG_WM_DESTROY( wmDestroy )
		MSG_WM_SIZE( wmSize )
		MESSAGE_HANDLER( WM_PRESENT, wmPresent )
		MESSAGE_HANDLER( WM_TRANSITION, wmTransition )
	END_MSG_MAP()

private:
	HWND m_hWnd = nullptr;

	int wmCreate( LPCREATESTRUCT lpCreateStruct );

	HRESULT createDevice();
	void destroyDevice();
	void wmDestroy();
	HRESULT wmSize( UINT nType, CSize size );

	LRESULT wmPresent( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled );
	LRESULT wmTransition( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& handled );

	HRESULT sendMessageTimeout( UINT wm, const void* wParam );

	HRESULT doPresent();

public:

	HRESULT presentSingle( const RenderTarget& src );

	HRESULT presentTransition( const RenderTarget& t1, const RenderTarget& t2, int trans, float sintrans );
};