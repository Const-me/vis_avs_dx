#include "stdafx.h"
#include "Picture.h"

extern HINSTANCE g_hInstance;

HRESULT Picture::loadResource( int id )
{
	HRSRC res = FindResourceA( g_hInstance, MAKEINTRESOURCEA( id ), "GIF" );
	if( nullptr == res )
		return getLastHr();

	const DWORD cb = SizeofResource( g_hInstance, res );
	HGLOBAL data = LoadResource( g_hInstance, res );
	if( nullptr == data )
		return getLastHr();
	void* pv = LockResource( data );

	CComPtr<IStream> stm = SHCreateMemStream( (const BYTE*)pv, cb );
	if( !stm )
		return E_FAIL;

	CComPtr<IPicture> p;
	CHECK( OleLoadPicture( stm, 0, FALSE, IID_PPV_ARGS( &p ) ) );

	CHECK( p->get_Width( &width ) );
	CHECK( p->get_Height( &height ) );
	pic.Attach( p.Detach() );

	return S_OK;
}

HRESULT Picture::render( const DRAWITEMSTRUCT& ds )
{
	if( !pic )
		return E_POINTER;
	const CRect rc = ds.rcItem;
	return pic->Render( ds.hDC, rc.left, rc.top, rc.Width(), rc.Height(), 0, 0, width, height, nullptr );
}