#pragma once
#include "../shadersCode.h"
#include "../../../InteropLib/interop.h"

// Geometry + pixel shaders to render a collection of point sprites.
struct PointSpritesRender
{
	struct GsData : public Hlsl::Render::PointSpriteGS
	{
		template<class TAvs>
		HRESULT update( const TAvs& a )
		{
			const CSize screenSize = getRenderSize();
			if( screenSize == m_pixels )
				return S_FALSE;

			constexpr float px800 = 2;
			const float px = px800 * screenSize.cx * ( 1.0f / px800 );
			sizeInClipSpace.x = px / screenSize.cx;
			sizeInClipSpace.y = px / screenSize.cx;
			m_pixels = screenSize;
			return S_OK;
		}

	private:
		CSize m_pixels;
	};

	static ID3D11PixelShader* pixelShader()
	{
		return StaticResources::pointSprite;
	}
};