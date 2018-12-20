inline float trigArgument( double x )
{
    const double pi2 = 3.141592653589793238 * 2;
    int div = (int) ( x / pi2 );
    if( x < 0 )
        div--;
    return (float) ( ( x - div * pi2 ) * pi2 );
}

inline float sin_d( double x )
{
    return sin( trigArgument( x ) );
}

inline float cos_d( double x )
{
    return cos( trigArgument( x ) );
}

inline float tan_d( double x )
{
    return tan( trigArgument( x ) );
}