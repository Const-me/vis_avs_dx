#include "VisCommon.fx"
#include "Polyline.fx"

#ifndef AVS_SHADER
static const float sampleV = 0.75f;
static const bool drawLines = false;
#define SHADER_GLOBALS
#define SHADER_CODE
#endif

SHADER_GLOBALS

sVertex1 main( uint id : SV_VertexID )
{
    const float3 color = stateFloat3( -3 );
    float red = color.x;
    float green = color.y;
    float blue = color.z;
    float x, y, skip, linesize;
    float i = 0.0f;
    {
SHADER_CODE
    }

    sVertex1 vert;
    vert.pos = float4( 0, 0, -1, 1 );
    vert.color = float4( 0, 0, 0, 0 );
    return vert;
}