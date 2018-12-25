#pragma once

// An interface to receive raw samples from the decoder.
__interface iSampleSink
{
	HRESULT haveSample( IMFSample* sample );
};