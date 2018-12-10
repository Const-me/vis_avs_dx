#pragma once

class RenderTarget;

CSize getRenderSize();

extern CComAutoCriticalSection renderLock;

HRESULT present( const RenderTarget& src );