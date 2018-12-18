#pragma once
// Interop API for the DxVisuals to consume.

class RenderTarget;

// Size of the viewport in pixels.
CSize getRenderSize();

extern CComAutoCriticalSection renderLock;

// Present the specified target on the GUI thread.
HRESULT presentSingle( const RenderTarget& src );
HRESULT presentTransition( const RenderTarget& t1, const RenderTarget& t2, int trans, float sintrans );