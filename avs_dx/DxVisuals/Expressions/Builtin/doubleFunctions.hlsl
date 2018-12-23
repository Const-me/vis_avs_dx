// Trigonometric stuff, use double-precision math to calculate val % 2*pi, then use float built-ins.

inline float wrap_double_2pi( double x )
{
    const double pi = 3.141592653589793238;
    const double _2pi = pi * 2;
    const double _2piinv = 1.0 / _2pi;
    int divInt;
    if( x >= 0 )
        divInt = (int) ( x * _2piinv );
    else
        divInt = -(int) ( -x * _2piinv );
    return (float) ( x - (double) divInt * _2pi );
}

inline float sin_d( double x )
{
    return sin( wrap_double_2pi( x ) );
}

inline float cos_d( double x )
{
    return cos( wrap_double_2pi( x ) );
}

inline float tan_d( double x )
{
    return tan( wrap_double_2pi( x ) );
}

// double-precision intrinsics that are missing from HLSL.
inline double abs_d( double x )
{
    return x >= 0 ? x : -x;
}

inline double floor_d( double x )
{
    return x >= 0 ? (int) ( x ) : -(int) ( -x );
}

inline double ceil_d( double x )
{
    return x >= 0 ? -(int) ( -x ) : (int) ( x );
}

inline double min_d( double a, double b )
{
    return a < b ? a : b;
}

inline double max_d( double a, double b )
{
    return a > b ? a : b;
}

[numthreads( 1, 1, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
    wrap_double_2pi( 0.5 );
}