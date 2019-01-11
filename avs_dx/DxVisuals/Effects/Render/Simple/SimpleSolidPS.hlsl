#include "VisCommon.hlsli"
#ifndef AVS_SHADER
static const float sampleV = 0.75f;
#endif

inline bool shouldDiscard( float val, float tc )
{
    const bool isWaveform = ( sampleV > 0.5 );
    if( isWaveform )
    {
        tc = tc * 2 - 1;
        return sign( val ) != sign( tc ) || abs( tc ) > abs( val );
    }
    return tc > val;
}

float4 main( float2 tc : TEXCOORD0, float4 color : COLOR0 ) : SV_Target
{
	// ConstructGSWithSO 
    const bool isWaveform = ( sampleV > 0.5 );
    float val = sampleVisData( float2( tc.x, sampleV ), isWaveform );
    if( shouldDiscard( val, tc.y ) )
    {
        discard;
			// http://jbrd.github.io/2008/02/14/hlsl-discard-doesnt-return.html
        return float4( 0, 0, 0, 0 );
    }
    return color;
}