#include "VisCommon.fx"
#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
static const uint dir = 0;
#endif
Texture2D<float4> texPrevFrame : register( BIND_PREV_FRAME );

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
	const float4 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );
    switch( dir )
    {
        case 0: // 2x brighter
            return saturate( color * 2.0 );
		case 1:
            return color * 0.5f;
    }
    return 0;
}