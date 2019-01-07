#include "stdafx.h"
#include "ProfilerWindow.h"

static const CPoint minSize{ 320, 320 };
static const CSize defaultSize{ 540, 480 };

HRESULT ProfilerWindow::create()
{
	constexpr DWORD style = ( WS_VISIBLE | WS_OVERLAPPEDWINDOW ) & ( ~WS_MAXIMIZEBOX );
	CRect rcDesktop;
	if( !SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesktop, FALSE ) )
		return getLastHr();

	CRect rcWindow{ rcDesktop.BottomRight() - defaultSize, defaultSize };
	if( !__super::Create( nullptr, &rcWindow, L"GPU Profiler", style ) )
		return getLastHr();
	m_text.Preallocate( 1024 );
	return S_OK;
}

int ProfilerWindow::wmCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( !m_font )
	{
		LOGFONT font = {};
		font.lfWeight = FW_REGULAR;
		wcscpy_s( font.lfFaceName, L"Consolas" );
		m_font.CreateFontIndirect( &font );
	}

	if( !m_backgroundBrush )
		m_backgroundBrush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );

	return TRUE;
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

bool updateProfilerGui( uint32_t frame, vector<sProfilerEntry>& entries )
{
	if( !isProfilerOpen() )
		return false;

	g_profiler.update( frame, entries );
	return true;
}

void ProfilerWindow::update( uint32_t frame, vector<sProfilerEntry>& entries )
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
	constexpr int locAverage = 24;
	constexpr int locCurrent = 36;

	CSLock __lock( m_cs );

	m_averages.update( m_entries );

	m_text = L"";
	CStringW str;
	str.Preallocate( 64 );
	for( size_t i = 0; i < m_entries.size(); i++ )
	{
		const sProfilerEntry& e = m_entries[ i ];
		// Indent according to level
		const int indent = e.level * indentSpaces;
		addSpaces( str, false, indent );

		str.AppendFormat( L"%S", e.measure );

		addSpaces( str, true, locAverage - str.GetLength() );

		str.AppendFormat( L"%.5f", m_averages[ i ] );

		addSpaces( str, true, locCurrent - str.GetLength() );

		str.AppendFormat( L"%.5f", e.milliseconds );

		m_text += str;
		if( i + 1 != m_entries.size() )
			m_text += L"\r\n";
	}
	InvalidateRect( nullptr, false );

	str.Format( L"GPU Profiler: frame %i", m_frame );
	SetWindowText( str );
	return 0;
}

void ProfilerWindow::wmDestroy()
{
	if( m_prevBitmap )
	{
		m_memDc.SelectFont( m_prevFont.Detach() );
		m_memDc.SelectBitmap( m_prevBitmap.Detach() );
		m_memBmp.DeleteObject();
		m_memDc.DeleteDC();
	}
}

void ProfilerWindow::wmSize( UINT nType, CSize size )
{
	switch( nType )
	{
	case SIZE_MAXIMIZED:
	case SIZE_RESTORED:
		break;
	default:
		return;
	}

	m_clientRect = CRect{ CPoint{ 0, 0 }, size };
	m_textRect = m_clientRect;
	m_textRect.left += 16;
	m_textRect.top += 4;

	wmDestroy();

	CDC dc = GetDC();
	m_memDc.CreateCompatibleDC( dc );
	m_memBmp.CreateCompatibleBitmap( dc, size.cx, size.cy );
	m_prevBitmap = m_memDc.SelectBitmap( m_memBmp );
	m_prevFont = m_memDc.SelectFont( m_font );

	m_memDc.SetBkMode( TRANSPARENT );
	m_memDc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );
}

void ProfilerWindow::wmPaint( HDC )
{
	// Double buffering to eliminate flicker: https://msdn.microsoft.com/en-us/library/ms969905.aspx
	m_memDc.FillRect( m_clientRect, m_backgroundBrush );

	constexpr UINT fmt = DT_LEFT | DT_TOP;
	m_memDc.DrawText( m_text, m_text.GetLength(), m_textRect, fmt );

	PAINTSTRUCT ps;
	CDCHandle dc = BeginPaint( &ps );
	dc.BitBlt( 0, 0, m_clientRect.Width(), m_clientRect.Height(), m_memDc, 0, 0, SRCCOPY );
	EndPaint( &ps );
	ValidateRect( nullptr );
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