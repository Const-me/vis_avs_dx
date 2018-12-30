#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#define AVS_RENDER_SIZE float2( 1280, 720 )
static const float outAmount = 1.15f;
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    uint2 pos = uint2( screenSpace.xy );
    const uint2 halfPos = pos / 2;
    if( 1 == ( halfPos.x & halfPos.y & 1 ) )
    {
        float2 f = (float2) pos - ( AVS_RENDER_SIZE * 0.5 );
        f = f * ( 1.0 / outAmount ) + ( AVS_RENDER_SIZE * 0.5 );
        pos = (uint2) f;
    }
    return texPrevFrame.Load( int3( int2( pos ), 0 ) );
}