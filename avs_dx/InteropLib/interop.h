#pragma once
// Interop API for the DxVisuals to consume.

class RenderTarget;

// Size of the viewport in pixels.
CSize getRenderSize();

extern CComAutoCriticalSection renderLock;

// Present the specified target on the GUI thread.
HRESULT present( const RenderTarget& src );