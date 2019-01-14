#ifndef AVS_SHADER
#define BIND_SOURCE u3
// <macro-resource name="bindInput" macro="BIND_SOURCE" type="uav" />
static const uint vertsToDraw = 1024;
#endif

ConsumeStructuredBuffer<float2> sourceBuffer : register(BIND_SOURCE);

float4 main( uint id : SV_VertexID ) : SV_Position
{
    if( id >= vertsToDraw )
        return float4( 0, 0, 1, -0.1 );
    return float4( sourceBuffer.Consume(), 0.5, 1 );
}