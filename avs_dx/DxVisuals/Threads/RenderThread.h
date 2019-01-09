#pragma once
#include <atltypes.h>
#include "ThreadBase.h"

class RenderThread : public ThreadBase
{
protected:

	RenderThread( winampVisModule * pModule ) :
		ThreadBase( pModule ) { }

	HRESULT startup();

	void renderFrame();

	void updateStats( const CSize &framePixels, float fps );
};