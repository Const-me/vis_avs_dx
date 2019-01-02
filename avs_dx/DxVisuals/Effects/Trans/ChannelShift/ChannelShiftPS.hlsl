#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
static const uint mode = 1;
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

#define IDC_GBR                         1018
#define IDC_BRG                         1019
#define IDC_RBG                         1020
#define IDC_BGR                         1021
#define IDC_GRB                         1022
#define IDC_RGB                         1183

inline float3 shift( float3 c )
{
    switch( mode )
    {
        case IDC_RBG:
            return c.rbg;
        case IDC_BRG:
            return c.brg;
        case IDC_BGR:
            return c.bgr;
        case IDC_GBR:
            return c.rbr;
        case IDC_GRB:
            return c.grb;
    }
    return c;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    return float4( shift( color ), 1 );
}