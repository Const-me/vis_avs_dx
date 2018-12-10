#pragma once

inline HRESULT getLastHr() { return HRESULT_FROM_WIN32( GetLastError() ); }

#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, #hr ); return __hr; } }

inline void csSetUav( ID3D11UnorderedAccessView* uav, UINT slot )
{
	context->CSSetUnorderedAccessViews( slot, 1, &uav, nullptr );
}

inline void omSetTarget( ID3D11RenderTargetView* rtv )
{
	context->OMSetRenderTargets( 1, &rtv, nullptr );
}

inline void omClearTargets()
{
	context->OMSetRenderTargets( 0, nullptr, nullptr );
}

inline void setShaders( ID3D11VertexShader* vs, ID3D11GeometryShader* gs, ID3D11PixelShader* ps )
{
	context->VSSetShader( vs, nullptr, 0 );
	context->GSSetShader( gs, nullptr, 0 );
	context->PSSetShader( ps, nullptr, 0 );
}

inline void iaClearBuffers()
{
	context->IASetVertexBuffers( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, nullptr, nullptr, nullptr );
}