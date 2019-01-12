#include "stdafx.h"
#include "AboutDialog.h"
#pragma comment(lib, "Mincore.lib")

class DpiScaler
{
	static Vector2 convertSize( const CSize &a )
	{
		return Vector2( (float)a.cx, (float)a.cy );
	}
	static CSize convertSize( const Vector2 &a )
	{
		return CSize( lroundf( a.x ), lroundf( a.y ) );
	}

	static Vector2 getScaleFactor( HWND w )
	{
		constexpr int testSize = 1000;
		CRect rcTest{ CPoint( 0 ), CSize{ testSize, testSize } };
		MapDialogRect( w, &rcTest );
		return Vector2( (float)rcTest.Width() * ( 1.0f / 3000.0f ), (float)rcTest.Height() * ( 1.0f / 3125.0f ) );

		// const CSize unit( (DWORD)GetDialogBaseUnits() );
		// return convertSize( unit ) * Vector2( 1.0f / 10, 1.0f / 20 );
	}

	const HWND hWnd;
	const Vector2 mul;

public:

	DpiScaler( HWND w ) : hWnd( w ), mul( getScaleFactor( w ) ) { }

	void apply( int nIDDlgItem, const CSize &refSize, bool moveRightEdge )
	{
		HWND item = GetDlgItem( hWnd, nIDDlgItem );
		CRect rc;
		GetWindowRect( item, &rc );
		::MapWindowPoints( HWND_DESKTOP, hWnd, (LPPOINT)&rc, 2 );
		const Vector2 size = convertSize( refSize );
		const CSize sz = convertSize( size * mul );

		rc.top = rc.bottom - sz.cy;
		if( moveRightEdge )
			rc.right = rc.left + sz.cx;
		else
			rc.left = rc.right - sz.cx;

		MoveWindow( item, rc.left, rc.top, rc.Width(), rc.Height(), FALSE );
	}
};

extern HINSTANCE g_hInstance;

HRESULT tryGetVersion( CString& res )
{
	// https://stackoverflow.com/a/13942403/126995
	// https://stackoverflow.com/a/34348748/126995
	const HMODULE hInst = (HMODULE)g_hInstance;

	const HRSRC hResInfo = FindResource( hInst, MAKEINTRESOURCE( 1 ), RT_VERSION );
	if( !hResInfo ) return getLastHr();

	const DWORD dwSize = SizeofResource( hInst, hResInfo );
	if( 0 == dwSize ) return getLastHr();

	const HGLOBAL hResData = LoadResource( hInst, hResInfo );
	if( nullptr == hResData ) return getLastHr();

	const uint8_t* const ptr = (const uint8_t*)LockResource( hResData );
	vector<uint8_t> copy;
	copy.assign( ptr, ptr + dwSize );
	FreeResource( hResData );

	VS_FIXEDFILEINFO *lpFfi;
	UINT uLen;
	if( !VerQueryValue( copy.data(), L"\\", (LPVOID*)&lpFfi, &uLen ) )
		return E_FAIL;

	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;

	DWORD dwLeftMost = HIWORD( dwFileVersionMS );
	DWORD dwSecondLeft = LOWORD( dwFileVersionMS );
	DWORD dwSecondRight = HIWORD( dwFileVersionLS );
	DWORD dwRightMost = LOWORD( dwFileVersionLS );

	res.AppendFormat( L", version %i.%i.%i.%i", dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost );
	return S_OK;
}

BOOL AboutDialog::OnInitDialog( CWindow wndFocus, LPARAM lInitParam )
{
	CString verStr = L"AVS DirectX";
	tryGetVersion( verStr );
	SetDlgItemText( IDC_ABOUT_VERSION, verStr );

	ua.loadResource( IDR_GIF_UA );
	me.loadResource( IDR_GIF_ME );

	if( !( ua  && me ) )
		return TRUE;

	DpiScaler scaler{ m_hWnd };
	scaler.apply( IDC_FLAG_UA, CSize{ 48, 32 }, true );
	scaler.apply( IDC_FLAG_ME, CSize{ 56, 28 }, false );

	return TRUE;
}

void AboutDialog::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lp )
{
	switch( nIDCtl )
	{
	case IDC_FLAG_UA:
		ua.render( *lp );
		return;
	case IDC_FLAG_ME:
		me.render( *lp );
		return;
	}
}