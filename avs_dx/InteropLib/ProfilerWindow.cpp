#include "stdafx.h"
#include "ProfilerWindow.h"
#include "Utils/WTL/atlctrls.h"

static const CPoint minSize{ 320, 320 };
static const CSize defaultSize{ 480, 480 };

class ProfilerWindow : public CWindowImpl<ProfilerWindow, CWindow, CFrameWinTraits>
{
	static constexpr UINT WM_UPDATE_DATA = WM_USER + 1337;

public:
	DECLARE_WND_CLASS( L"AVS DX Profiler" );

	BEGIN_MSG_MAP_EX( ProfilerWindow )
		MSG_WM_CREATE( wmCreate )
		// MSG_WM_DESTROY( wmDestroy )
		MSG_WM_SIZE( wmSize )
		MSG_WM_GETMINMAXINFO( wmGetMinMaxInfo )
		MESSAGE_HANDLER( WM_UPDATE_DATA, wmUpdate )

		END_MSG_MAP()

	void update( uint32_t frame, std::vector<sProfilerEntry>& entries );

	HRESULT create();

private:
	int wmCreate( LPCREATESTRUCT lpCreateStruct );
	// void wmDestroy();
	void wmSize( UINT nType, CSize size );
	void wmGetMinMaxInfo( LPMINMAXINFO lpMMI );
	LRESULT wmUpdate( UINT, WPARAM, LPARAM, BOOL );

	CComAutoCriticalSection m_cs;
	uint32_t m_frame;
	std::vector<sProfilerEntry> m_entries;

	CListBox m_items;
	CFont m_font;
};

HRESULT ProfilerWindow::create()
{
	constexpr DWORD style = ( WS_VISIBLE | WS_OVERLAPPEDWINDOW ) & ( ~WS_MAXIMIZEBOX );
	CRect rcDesktop;
	if( !SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesktop, FALSE ) )
		return getLastHr();

	CRect rcWindow{ rcDesktop.BottomRight() - defaultSize, defaultSize };
	if( !__super::Create( nullptr, &rcWindow, L"GPU Profiler", style ) )
		return getLastHr();
	return S_OK;
}

int ProfilerWindow::wmCreate( LPCREATESTRUCT lpCreateStruct )
{
	constexpr DWORD style = LBS_HASSTRINGS | LBS_NOSEL | WS_VSCROLL | WS_VISIBLE | WS_CHILD;
	m_items.Create( m_hWnd, nullptr, nullptr, style );

	LOGFONT font = {};
	font.lfWeight = FW_REGULAR;
	wcscpy_s( font.lfFaceName, L"Consolas" );
	m_font.CreateFontIndirect( &font );
	m_items.SetFont( m_font );
	return TRUE;
}

void ProfilerWindow::wmSize( UINT nType, CSize size )
{
	CRect rc{ CPoint{ 0, 0 }, size };
	m_items.MoveWindow( rc );
}

void ProfilerWindow::wmGetMinMaxInfo( LPMINMAXINFO lpMMI )
{
	lpMMI->ptMinTrackSize = minSize;
	lpMMI->ptMaxSize.x = 640;
}

static ProfilerWindow g_profiler;

bool isProfilerEnabled()
{
#if GPU_PROFILE
	return true;
#else
	return false;
#endif
}

bool isProfilerOpen()
{
#if !GPU_PROFILE
	return false;
#endif
	return g_profiler.IsWindow();
}

bool updateProfilerGui( uint32_t frame, std::vector<sProfilerEntry>& entries )
{
	if( !isProfilerOpen() )
		return false;

	g_profiler.update( frame, entries );
	return true;
}

void ProfilerWindow::update( uint32_t frame, std::vector<sProfilerEntry>& entries )
{
	{
		CSLock __lock( m_cs );
		m_frame = frame;
		m_entries.swap( entries );
	}
	g_profiler.PostMessage( WM_UPDATE_DATA );
}

inline void addSpaces( CString &s, bool append, int n )
{
	if( n <= 0 )
	{
		if( !append )
			s = L"";
		return;
	}

	// https://stackoverflow.com/a/9448093/126995
	if( append )
		s.AppendFormat( L"%*s", n, L"" );
	else
		s.Format( L"%*s", n, L"" );
}

LRESULT ProfilerWindow::wmUpdate( UINT, WPARAM, LPARAM, BOOL )
{
	if( !IsWindow() )
		return 0;

	constexpr int indentSpaces = 2;
	constexpr int minNameLength = 16;

	CSLock __lock( m_cs );
	m_items.ResetContent();
	CStringW str;
	str.Preallocate( 64 );
	for( auto e : m_entries )
	{
		// Indent according to level
		const int indent = e.level * indentSpaces;
		addSpaces( str, false, indent );

		str.AppendFormat( L"%S", e.measure );

		addSpaces( str, true, indent + minNameLength - str.GetLength() );

		if( e.milliseconds < 0 || e.milliseconds>1000 )
			str += L"n/a";
		else
			str.AppendFormat( L"%.5f", e.milliseconds );

		m_items.AddString( str );
	}

	str.Format( L"GPU Profiler: frame %i", m_frame );
	SetWindowText( str );
	return 0;
}

void profilerOpen()
{
#if !GPU_PROFILE
	return;
#endif

	if( isProfilerOpen() )
	{
		g_profiler.ShowWindow( SW_RESTORE );
		SetForegroundWindow( g_profiler );
		return;
	}
	g_profiler.create();
}

void profilerClose()
{
#if !GPU_PROFILE
	return;
#endif

	if( !isProfilerOpen() )
		return;
	g_profiler.DestroyWindow();
}