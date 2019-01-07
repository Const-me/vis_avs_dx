#pragma once
#include <Effects/Common/EffectImpl.hpp>
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

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	// Open/close the video. Both called from GUI thread.
	HRESULT open( const char* selection );
	HRESULT close();

private:

	AvsState& avs;
	CComPtr<iPlayer> m_player;
	FadeOut m_fade;
};