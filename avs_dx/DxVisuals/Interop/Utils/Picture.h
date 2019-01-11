#pragma once

class Picture
{
	CComPtr<IPicture> pic;
	OLE_XSIZE_HIMETRIC width;
	OLE_YSIZE_HIMETRIC height;

public:

	operator bool() const { return nullptr != pic; }

	HRESULT loadResource( int id );

	HRESULT render( const DRAWITEMSTRUCT& ds );
};