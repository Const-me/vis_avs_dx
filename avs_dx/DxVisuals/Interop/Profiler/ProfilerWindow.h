#pragma once
#include "../profilerApi.h"
#include "ProfilerAverages.h"
#include "../wtl.h"
#include "TableData.h"

class ProfilerWindow : public CWindowImpl<ProfilerWindow, CWindow, CFrameWinTraits>
{
	static constexpr UINT WM_UPDATE_DATA = WM_USER + 1337;
	static constexpr WORD menuSaveLog = 1;
	TableData m_file;

public:
	DECLARE_WND_CLASS( L"AVS DX Profiler" );

	BEGIN_MSG_MAP_EX( ProfilerWindow )
		MSG_WM_CREATE( wmCreate )
		MSG_WM_DESTROY( wmDestroy );
		MSG_WM_SIZE( wmSize );
		MSG_WM_PAINT( wmPaint );
		MSG_WM_CLOSE( vmClose );
		MSG_WM_GETMINMAXINFO( wmGetMinMaxInfo )
		MESSAGE_HANDLER( WM_UPDATE_DATA, wmUpdate )
		MSG_WM_SYSCOMMAND( wmSysCommand )

		END_MSG_MAP()

	void update( uint32_t frame, vector<sProfilerEntry>& entries );

	HRESULT create();

private:
	int wmCreate( LPCREATESTRUCT lpCreateStruct );
	void wmDestroy();
	void wmGetMinMaxInfo( LPMINMAXINFO lpMMI );
	LRESULT wmUpdate( UINT, WPARAM, LPARAM, BOOL );
	void wmSize( UINT nType, CSize size );
	void wmPaint( HDC );
	void wmSysCommand( UINT nID, CPoint point );
	void vmClose();

	CComAutoCriticalSection m_cs;
	uint32_t m_frame;
	vector<sProfilerEntry> m_entries;
	ProfilerAverages m_averages;

	CString m_text;

	CFont m_font;
	CBrush m_backgroundBrush;

	CRect m_clientRect, m_textRect;

	CDC m_memDc;
	CBitmap m_memBmp;
	CBitmapHandle m_prevBitmap;
	CFontHandle m_prevFont;
};