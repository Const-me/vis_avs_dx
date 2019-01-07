#pragma once
#include "../Utils/ByteRange.hpp"

template<class I>
inline HRESULT createShader( ByteRange dxbc, CComPtr<I>& result );

#define CREATE_SHADER( STAGE )                                                          \
template<>                                                                              \
inline HRESULT createShader( ByteRange dxbc, CComPtr<ID3D11##STAGE##Shader>& result )   \
{	return device->Create##STAGE##Shader( dxbc.data, dxbc.size, nullptr, &result ); }

CREATE_SHADER( Compute )
CREATE_SHADER( Vertex )
CREATE_SHADER( Geometry )
CREATE_SHADER( Pixel )

#undef CREATE_SHADER

template<class I>
inline HRESULT createShader( const vector<uint8_t>& dxbc, CComPtr<I>& result )
{
	ByteRange r{ dxbc.data(), dxbc.size() };
	return createShader( r, result );
}