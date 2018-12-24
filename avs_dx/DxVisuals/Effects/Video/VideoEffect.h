#pragma once
#include "../EffectImpl.hpp"
#include <Vfw.h>
using namespace Hlsl::Video;

struct VideoStructs
{
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
	};

	using StateData = EmptyStateData;

	class VsData : public VideoVS
	{
	public:
		HRESULT updateAvs( const AvsState& ass );
	};

};

class VideoEffect : public EffectBase1<VideoStructs>
{
public:
	VideoEffect( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};