#pragma once
#include "SourceData.h"
#include "GlobalBuffers.h"
#include "BlendModes.h"

namespace StaticResources
{
	HRESULT create();
	void destroy();

	extern CComPtr<ID3D11VertexShader> fullScreenTriangle;
	extern CComPtr<ID3D11PixelShader> copyTexture;
	extern CComPtr<ID3D11VertexShader> fullScreenTriangleTC;
	extern CComPtr<ID3D11PixelShader> copyTextureBilinear;
	extern CComPtr<ID3D11PixelShader> fadeFramePS;

	extern CComPtr<ID3D11SamplerState> sampleBilinear, sampleNearest;
	extern CComPtr<ID3D11BlendState> blendPremultipliedAlpha;
	extern CComPtr<ID3D11BlendState> blendAdditive;
	extern CComPtr<ID3D11BlendState> blendCustom;

	extern CComPtr<ID3D11ShaderResourceView> blackTexture;
	extern CComPtr<ID3D11RasterizerState> rsDisableCulling;
	// This particular input layout is used by too many effects, all move effects use them. That's why created here.
	HRESULT createLayout( const vector<uint8_t>& dxbc );
	extern CComPtr<ID3D11InputLayout> layoutPos2Tc2;

	extern SourceData sourceData;

	extern GlobalBuffers globalBuffers;
	extern BlendModes globalBlendModes;

	// The rest of the layouts are created on demand and go to a hash map.
	HRESULT cacheInputLayout( const void* key, const D3D11_INPUT_ELEMENT_DESC *desc, UINT count, const vector<uint8_t>& dxbc );
	HRESULT bindCachedInputLayout( const void* key );
};