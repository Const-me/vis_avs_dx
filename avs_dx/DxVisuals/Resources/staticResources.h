#pragma once
#include "SourceData.h"

namespace StaticResources
{
	HRESULT create();
	void destroy();

	HRESULT createLayout( const std::vector<uint8_t>& dxbc );

	extern CComPtr<ID3D11VertexShader> fullScreenTriangle;
	extern CComPtr<ID3D11PixelShader> copyTexture;
	extern CComPtr<ID3D11PixelShader> pointSprite;
	extern CComPtr<ID3D11PixelShader> fadeFramePS;

	extern CComPtr<ID3D11SamplerState> sampleBilinear;
	extern CComPtr<ID3D11BlendState> blendPremultipliedAlpha;
	extern CComPtr<ID3D11BlendState> blendAdditive;
	extern CComPtr<ID3D11BlendState> blendCustom;

	extern CComPtr<ID3D11ShaderResourceView> blackTexture;
	extern CComPtr<ID3D11RasterizerState> rsDisableCulling;
	extern CComPtr<ID3D11InputLayout> layoutPos2Tc2;

	extern SourceData sourceData;
};