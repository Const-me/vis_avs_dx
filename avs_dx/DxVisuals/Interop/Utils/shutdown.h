#pragma once

// true if rendering thread is shutting down.
bool checkShutdown();

HRESULT shutdownThread();

void onRenderThreadShuttingDown( bool renderThread );