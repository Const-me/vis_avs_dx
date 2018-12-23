#pragma once

// String for AVS_RENDER_SIZE HLSL macro, e.g. "float2( 1280, 720 )"
const CStringA& getRenderSizeString();

__interface iResizeHandler
{
	void onRenderSizeChanged();
};

void subscribeHandler( iResizeHandler* pHandler );
void unsubscribeHandler( iResizeHandler* pHandler );

void callResizeHandlers();

// An abstract base class for iResizeHandler that automatically handles the subscription.
class ResizeHandler : public iResizeHandler
{
public:
	ResizeHandler();
	~ResizeHandler();
};