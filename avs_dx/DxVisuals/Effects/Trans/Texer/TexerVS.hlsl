#ifndef AVS_SHADER
#define BIND_SOURCE t0
// <macro-resource name="bindInput" macro="BIND_SOURCE" />
static const uint vertsToDraw = 1024;
#endif

struct sSprite
{
    float2 position;
    float3 color;
};
struct sVertex
{
    float4 position : SV_Position;
    float3 color : COLOR0;
};
StructuredBuffer<sSprite> sourceBuffer : register(BIND_SOURCE);

sVertex main( uint id : SV_VertexID )
{
    sVertex vert;

    if( id >= vertsToDraw )
    {
        vert.position = float4( 0, 0, 1, -0.1 );
        vert.color = 0;
    }
	else
    {
        sSprite s = sourceBuffer[ id ];
        vert.position = float4( s.position, 0.5, 1 );
        vert.color = s.color;
    }
    return vert;
}