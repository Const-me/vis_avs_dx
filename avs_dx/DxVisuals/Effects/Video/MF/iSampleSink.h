#pragma once
#include <mfobjects.h>

// An interface to receive raw samples from the decoder.
__interface iSampleSink
{
	// S_OK to request another sample, S_FALSE to not request.
	HRESULT haveSample( IMFSample* sample );
};