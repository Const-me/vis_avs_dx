#ifndef AVS_SHADER
// A geometry shader that expands points into point sprites.
static const float2 sizeInPixels = float2( 8, 8 );
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif

struct sIn
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

struct sOut
{
    float2 tc : TEXCOORD0;
    float4 color : COLOR0;
    float4 pos : SV_Position;
};

[maxvertexcount( 4 )]
void main( point sIn input[1], inout TriangleStream<sOut> output )
{
    const float4 color = input[ 0 ].color;
    const float4 pos = input[ 0 ].pos;
    const float2 halfSize = sizeInPixels / AVS_RENDER_SIZE;

    [unroll]
	for( int i = 0; i < 4; i++ )
    {
        const float x = ( i & 1 ) * 2.0 - 1.0;
        const float y = 1.0 - ( i & 2 );
        sOut r;
        r.pos = pos;
        r.pos.xy += float2( x, y ) * halfSize * pos.ww;
        r.tc = float2( x, y );
        r.color = color;
        output.Append( r );
    }
}