#pragma once
#include "../EffectImpl.hpp"
#include <Vfw.h>
#include "MF/playerApi.h"
#include "../List/Blender.h"
#include <Utils/FadeOut.h>

class VideoEffect : public EffectBase
{
public:
	struct AvsState
	{
		int enabled;
		char ascName[ MAX_PATH ];
		int lastWidth, lastHeight;
		HANDLE hDrawDib;
		PAVISTREAM PAVIVideo;
		PGETFRAME PgetFrame;
		HBITMAP hRetBitmap;
		HBITMAP hOldBitmap;
		HDC hDesktopDC;
		HDC hBitmapDC;
		LPBITMAPINFOHEADER lpFrame;
		BITMAPINFO bi;
		int blend, blendavg, adapt, persist;
		int loaded, rendering;
		int lFrameIndex;
		int length;
		unsigned int speed;
		unsigned int lastspeed;
		int *old_image, old_image_w, old_image_h;

		eBlendMode blendMode() const;
	};

	VideoEffect( AvsState *pState );
	~VideoEffect();

	const Metadata& metadata() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	// Open/close the video. Both called from GUI thread.
	HRESULT open( const char* selection );
	HRESULT close();

private:

	AvsState& avs;
	CComPtr<iPlayer> m_player;
	FadeOut m_fade;
};