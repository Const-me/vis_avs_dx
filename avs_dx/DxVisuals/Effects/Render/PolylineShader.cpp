#include "stdafx.h"
#include "PolylineShader.h"

extern int g_line_blend_mode;

HRESULT updateThickness( float& val )
{
	int width = ( ( g_line_blend_mode >> 16 ) & 0xFF );
	if( 0 == width )
		width = 1;
	width *= 2;

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

HRESULT updateLineShaders( Shader<PolylineShaderGS>& geometry, Shader<PolylineShaderPS>& pixel )
{
	BoolHr hr;
	if( S_OK == geometry.data().updateAvs( nullptr ) )
	{
		geometry.setUpdated();
		geometry.compile( 0 );
		hr = true;
	}
	if( S_OK == pixel.data().updateAvs( nullptr ) )
	{
		pixel.setUpdated();
		pixel.compile( 0 );
		hr = true;
	}
	return hr;
}