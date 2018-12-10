#pragma once

template<class I>
inline HRESULT createShader( const std::vector<uint8_t>& dxbc, CComPtr<I>& result );

template<>
inline HRESULT createShader( const std::vector<uint8_t>& dxbc, CComPtr<ID3D11VertexShader>& result )
{
	return device->CreateVertexShader( dxbc.data(), dxbc.size(), nullptr, &result );
}
template<>
inline HRESULT createShader( const std::vector<uint8_t>& dxbc, CComPtr<ID3D11PixelShader>& result )
{
	return device->CreatePixelShader( dxbc.data(), dxbc.size(), nullptr, &result );
}
template<>
inline HRESULT createShader( const std::vector<uint8_t>& dxbc, CComPtr<ID3D11GeometryShader>& result )
{
	return device->CreateGeometryShader( dxbc.data(), dxbc.size(), nullptr, &result );
}
template<>
inline HRESULT createShader( const std::vector<uint8_t>& dxbc, CComPtr<ID3D11ComputeShader>& result )
{
	return device->CreateComputeShader( dxbc.data(), dxbc.size(), nullptr, &result );
}