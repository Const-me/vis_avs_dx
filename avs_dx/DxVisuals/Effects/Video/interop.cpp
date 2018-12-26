#include "stdafx.h"
#include "interop.h"
#include <../InteropLib/Utils/WTL/atlapp.h>
#include <../InteropLib/Utils/WTL/atlmisc.h>
#include <../InteropLib/Utils/WTL/atlctrls.h>
#include <atlpath.h>
#include "../includeDefs.h"
#include <shobjidl.h>
#include <shlguid.h>

namespace
{
	bool isVideoFileExtension( const CString& ext )
	{
		return ext == L".mp4" || ext == L".mpeg" || ext == L".mkv" || ext == L".avi" || ext == L".wmv";
	}

	// Get lower-case extension including the dot, e.g. ".mp4"
	CString getExtension( const CPath &path )
	{
		CString ext = path.GetExtension();
		ext.MakeLower();
		return ext;
	}

	// https://stackoverflow.com/q/22986845/126995
	HRESULT resolveShortcutTarget( HWND wnd, const CString& lnk, CString& target )
	{
		// Get a pointer to the IShellLink interface. It is assumed that CoInitialize has already been called. 
		CComPtr<IShellLink> psl;
		CHECK( psl.CoCreateInstance( CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER ) );

		// Get a pointer to the IPersistFile interface. 
		CComPtr<IPersistFile> ppf;
		CHECK( psl->QueryInterface( IID_PPV_ARGS( &ppf ) ) );

		// Load the shortcut. 
		CHECK( ppf->Load( lnk, STGM_READ ) );

		// Resolve the link. 
		CHECK( psl->Resolve( wnd, 0 ) );

		// Get the path to the link target. 
		const HRESULT hr = psl->GetPath( target.GetBufferSetLength( MAX_PATH ), MAX_PATH, nullptr, 0 );
		target.ReleaseBuffer();
		return hr;
	}
}

int initVideoCombobox( HWND wndDialog, HWND wndComboBox, char *selectedName )
{
	CComboBox cb{ wndComboBox };
	cb.ResetContent();

	CString gp{ g_path };
	CPath path{ gp };
	path.Append( L"*.*" );

	CFindFile ff;
	if( !ff.FindFile( path ) )
		return 0;

	int res = 0;
	do
	{
		if( ff.IsDots() )
			continue;

		path.m_strPath = ff.GetFilePath();
		CString ext = getExtension( path );

		if( !isVideoFileExtension( ext ) )
		{
			if( ext != ".lnk" )
				continue;
			if( FAILED( resolveShortcutTarget( wndDialog, path.m_strPath, path.m_strPath ) ) )
				continue;
			ext = path.GetExtension();
			ext.MakeLower();
			if( !isVideoFileExtension( ext ) )
				continue;
		}

		CStringW w = ff.GetFileName();
		CStringA a = w.operator LPCTSTR();
		const int ind = SendMessageA( wndComboBox, CB_ADDSTRING, 0, (LPARAM)cstr( a ) );
		if( a == selectedName )
			cb.SetCurSel( ind );

	} while( ff.FindNextFile() );
	return res;
}

extern HWND hwnd_WinampParent;

HRESULT getVideoFilePath( const char *selection, CString& result )
{
	CPath path{ CString{ g_path } };
	path.Append( CString{ selection } );

	CString ext = getExtension( path );
	if( isVideoFileExtension( ext ) )
	{
		if( !path.FileExists() )
			return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );
		result = path.m_strPath;
		return S_OK;
	}

	if( ext == ".lnk" )
	{
		CHECK( resolveShortcutTarget( hwnd_WinampParent, path, result ) );
		path.m_strPath = result;
		if( !path.FileExists() )
			return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

		ext = getExtension( path );
		if( isVideoFileExtension( ext ) )
			return S_OK;
	}
	return E_INVALIDARG;
}