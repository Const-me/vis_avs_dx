#include "FrameGlobalData.fx"

#ifndef AVS_SHADER
#define SIMPLE_READ_V 0.75f // center channel waveform
#define SIMPLE_ANALYZER true
#endif

ByteAddressBuffer effectStates : register(t1);

float4 main(float2 tc : TEXCOORD0) : SV_Target
{
    float val = sampleVisData(float2(tc.x, SIMPLE_READ_V));
    float colorMul = 0;
    if (SIMPLE_ANALYZER)
    {
		// Spectrum
        if (val < tc.y)
        {
            discard;
			// http://jbrd.github.io/2008/02/14/hlsl-discard-doesnt-return.html
            return float4(0, 0, 0, 0);
        }
    }
    else
    {
		// Waveform
        if (sign(val) != sign(tc.y) || abs(val) < abs(tc.y))
        {
            discard;
            return float4(0, 0, 0, 0);
        }
    }
    return asfloat(effectStates.Load4(EFFECT_STATE));
}