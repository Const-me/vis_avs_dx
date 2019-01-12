#pragma once
#include <Utils/resizeHandler.h>
#include <Hlsl/Defines.h>

// If the shader code has AVS_RENDER_SIZE or AVS_PIXEL_PARTICLES, set corresponding defines and subscribe the handler for change notifications. Otherwise unsubscribe.
bool setSizeDefines( const CStringA& hlsl, Hlsl::Defines &def, iResizeHandler* pThis );