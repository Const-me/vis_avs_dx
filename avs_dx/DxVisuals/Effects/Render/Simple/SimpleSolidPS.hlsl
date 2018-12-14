#include "VisCommon.fx"
#ifndef AVS_SHADER
static const float SIMPLE_READ_V = 0.75f;
static const bool SIMPLE_ANALYZER = true;
#endif

float4 main( float2 tc : TEXCOORD0, float4 color : COLOR0 ) : SV_Target
{
    float val = sampleVisData( float2( tc.x, SIMPLE_READ_V ), SIMPLE_READ_V > 0.5 );
    float colorMul = 0;
    if( SIMPLE_ANALYZER )
    {
		// Spectrum
        if( val < tc.y )
        {
            discard;
			// http://jbrd.github.io/2008/02/14/hlsl-discard-doesnt-return.html
            return float4( 0, 0, 0, 0 );
        }
    }
    else
    {
		// Waveform
        if( sign( val ) != sign( tc.y ) || abs( val ) < abs( tc.y ) )
        {
            discard;
            return float4( 0, 0, 0, 0 );
        }
    }
    return color;
}