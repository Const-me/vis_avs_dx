#pragma once
#include "RenderThread.h"

// Render thread is where the rendering happens. It's a separate one so the visualization runs regardless of user messing with the GUI controls.
// To present the result, this thread calls SendMessage() to call into the GUI thread.
class RenderThreadImpl : public RenderThread
{
private:
	friend class AvsThreads;

	static inline const char* dbgGetThreadName() { return "AVS Render thread"; }

	RenderThreadImpl( winampVisModule * pModule );

	~RenderThreadImpl();

	HRESULT initialize() override;

	HRESULT run() override;

	volatile long m_ThreadQuit = 0;

public:

	void postQuitmessage() override
	{
		InterlockedExchange( &m_ThreadQuit, (LONG)TRUE );
	}
};