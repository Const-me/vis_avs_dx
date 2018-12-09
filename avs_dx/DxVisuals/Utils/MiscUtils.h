#pragma once

inline HRESULT getLastHr() { return HRESULT_FROM_WIN32( GetLastError() ); }

#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, #hr ); return __hr; } }

inline void csSetUav( ID3D11UnorderedAccessView* uav, UINT slot )
{
	context->CSSetUnorderedAccessViews( slot, 1, &uav, nullptr );
}