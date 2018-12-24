#include "stdafx.h"
#include "VideoEffect.h"
#include "interop.h"

IMPLEMENT_EFFECT( VideoEffect, C_AVIClass );

// Start playing the video
HRESULT videoOpen( iRootEffect* pEffect, const char *selection )
{
	VideoEffect* const pfx = dynamic_cast<VideoEffect*>( pEffect );
	if( nullptr == pfx )
		return E_INVALIDARG;
	return pfx->open( selection );
}

// Close the video
HRESULT videoClose( iRootEffect* pEffect )
{
	VideoEffect* const pfx = dynamic_cast<VideoEffect*>( pEffect );
	if( nullptr == pfx )
		return E_INVALIDARG;
	return pfx->close();
}

// Get full path from the combobox selection
HRESULT getVideoFilePath( const char *selection, CString& result );

HRESULT VideoEffect::open( const char* selection )
{
	CString target;
	const HRESULT hr = getVideoFilePath( selection, target );
	if( FAILED( hr ) )
	{
		close();
		return hr;
	}
	return E_NOTIMPL;
}

HRESULT VideoEffect::close()
{
	return E_NOTIMPL;
}

HRESULT VideoStructs::VsData::updateAvs( const AvsState& ass )
{
	return S_FALSE;
}

HRESULT VideoEffect::render( bool isBeat, RenderTargets& rt )
{
	return E_NOTIMPL;
}