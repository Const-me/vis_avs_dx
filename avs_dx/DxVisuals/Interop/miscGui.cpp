#include "stdafx.h"
#include "miscGui.h"
#include "wtl.h"
#include <../../avs/vis_avs/resource.h>

extern HINSTANCE g_hInstance;

HRESULT setupTreeIcons( HWND wndTree )
{
	const CSize szIcon{ GetSystemMetrics( SM_CXSMICON ), GetSystemMetrics( SM_CYSMICON ) };

	CImageListManaged imgList;
	imgList.Attach( ImageList_Create( szIcon.cx, szIcon.cy, ILC_COLOR32, 2, 2 ) );
	if( !imgList )
		return getLastHr();

	HICON hi;

	hi = nullptr;
	CHECK( LoadIconWithScaleDown( g_hInstance, MAKEINTRESOURCE( IDI_NODX ), szIcon.cx, szIcon.cy, &hi ) );
	imgList.AddIcon( hi );

	hi = nullptr;
	CHECK( LoadIconWithScaleDown( g_hInstance, MAKEINTRESOURCE( IDI_DX ), szIcon.cx, szIcon.cy, &hi ) );
	imgList.AddIcon( hi );
	// You do not need to destroy hicon if it was loaded by the LoadIcon function; the system automatically frees an icon resource when it is no longer needed.

	CTreeViewCtrl tree = wndTree;
	tree.SetImageList( imgList.Detach() );
	return S_OK;
}