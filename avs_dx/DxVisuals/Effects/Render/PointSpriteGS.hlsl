#ifndef AVS_SHADER
// A geometry shader that expands points into point sprites. The points must be in clip space, they must have position and color attributes.
static const float2 sizeInClipSpace = float2( 0.01f, 0.01f * 16 / 9 );
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
    // const float4 color = input[ 0 ].color;
	// Debug code below: use white color
    const float4 color = float4( 1, 1, 1, 1 );
    const float4 pos = input[ 0 ].pos;

    [unroll]
	for( int i = 0; i < 4; i++ )
    {
        const float x = ( i & 1 ) * 2.0 - 1.0;
        const float y = 1.0 - ( i & 2 );
        sOut r;
        r.pos = pos;
        r.pos.xy += float2( x, y ) * sizeInClipSpace;
        r.tc = float2( x, y );
        r.color = color;
        output.Append( r );
    }
}