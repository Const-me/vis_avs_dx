#pragma once

namespace StaticResources
{
	HRESULT create();
	void destroy();

	extern CComPtr<ID3D11VertexShader> fullScreenTriangle;
	extern CComPtr<ID3D11PixelShader> copyTexture;
};