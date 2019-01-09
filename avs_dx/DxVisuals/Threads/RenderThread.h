#pragma once
#include "ThreadBase.h"

class RenderThread : public ThreadBase
{
protected:

	RenderThread( winampVisModule * pModule ) :
		ThreadBase( pModule ) { }

	HRESULT startup();

	void renderFrame();
};