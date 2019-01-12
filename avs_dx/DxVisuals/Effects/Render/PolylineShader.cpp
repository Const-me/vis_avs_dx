#include "stdafx.h"
#include "PolylineShader.h"

extern int g_line_blend_mode;
extern int cfg_fs_d;

HRESULT updateThickness( float& val )
{
	int width = ( ( g_line_blend_mode >> 16 ) & 0xFF );
	if( 0 == width )
		width = 1;

	if( !cfg_fs_d )
		width *= 4;

	const float res = (float)width;
	if( res == val )
		return S_FALSE;
	val = res;
	return S_OK;
}

HRESULT PolylineShaderGS::updateLineThickness()
{
	return updateThickness( sizeInPixels );
}

HRESULT PolylineShaderPS::updateLineThickness()
{
	return updateThickness( sizeInPixels );
}

HRESULT updateLineShaders( Shader<PolylineShaderGS>& gs, Shader<PolylineShaderPS>& ps )
{
	BoolHr hr;
	if( S_OK == gs.data().updateAvs( nullptr ) )
	{
		gs.compile( 0 );
		hr = true;
	}
	if( S_OK == ps.data().updateAvs( nullptr ) )
	{
		ps.compile( 0 );
		hr = true;
	}
	return hr;
}