#pragma once

bool checkShutdown();

HRESULT shutdownThread();

void onRenderThreadShuttingDown( bool renderThread );