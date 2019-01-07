#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include "../../List/Blender.h"
#include <Utils/FadeOut.h>

struct PictureStructs
{
	struct AvsState
	{
		int enabled;
		int width, height;
		HBITMAP hOldBitmap;
		HBITMAP hb;
		HBITMAP hb2;
		HDC hBitmapDC;
		HDC hBitmapDC2;
		int lastWidth, lastHeight;
		int blend, blendavg, adapt, persist;
		int ratio, axis_ratio;
		char ascName[ MAX_PATH ];
		int persistCount;

		eBlendMode blendMode() const;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Hlsl::Render::Picture::PicturePS;
};

class PictureEffect : public EffectBase1<PictureStructs>
{
public:
	PictureEffect( AvsState *pState ) : EffectBase1( pState ) { }

	DECLARE_EFFECT();

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	HRESULT open( const char* selection );
	HRESULT close();

private:

	CComAutoCriticalSection m_cs;
	CComPtr<ID3D11ShaderResourceView> m_pic;
	CString m_path;
	bool m_updated = false;
	FadeOut m_fade;
};