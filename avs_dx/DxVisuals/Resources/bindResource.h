#pragma once
#include "../Render/Stage.h"

// Constant buffers
template<eStage stage>
inline void bindConstantBuffer( UINT slot, ID3D11Buffer* buffer = nullptr );

template<>
inline void bindConstantBuffer<eStage::Compute>( UINT slot, ID3D11Buffer* buffer )
{
	context->CSSetConstantBuffers( slot, 1, &buffer );
}
template<>
inline void bindConstantBuffer<eStage::Vertex>( UINT slot, ID3D11Buffer* buffer )
{
	context->VSSetConstantBuffers( slot, 1, &buffer );
}
template<>
inline void bindConstantBuffer<eStage::Geometry>( UINT slot, ID3D11Buffer* buffer )
{
	context->GSSetConstantBuffers( slot, 1, &buffer );
}
template<>
inline void bindConstantBuffer<eStage::Pixel>( UINT slot, ID3D11Buffer* buffer )
{
	context->PSSetConstantBuffers( slot, 1, &buffer );
}

// Shader resources
template<eStage stage>
inline void bindResource( UINT slot, ID3D11ShaderResourceView* srv = nullptr );

template<>
inline void bindResource<eStage::Compute>( UINT slot, ID3D11ShaderResourceView* srv )
{
	context->CSSetShaderResources( slot, 1, &srv );
}
template<>
inline void bindResource<eStage::Vertex>( UINT slot, ID3D11ShaderResourceView* srv )
{
	context->VSSetShaderResources( slot, 1, &srv );
}
template<>
inline void bindResource<eStage::Geometry>( UINT slot, ID3D11ShaderResourceView* srv )
{
	context->GSSetShaderResources( slot, 1, &srv );
}
template<>
inline void bindResource<eStage::Pixel>( UINT slot, ID3D11ShaderResourceView* srv )
{
	context->PSSetShaderResources( slot, 1, &srv );
}

// UAVs are only supported by compute shaders
inline void bindUav( UINT slot, ID3D11UnorderedAccessView* uav = nullptr )
{
	context->CSSetUnorderedAccessViews( slot, 1, &uav, nullptr );
}