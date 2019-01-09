#include "stdafx.h"
#include "SourceBuffer.h"
#include "VIS.H"
#include "bpm.h"
using CSLock = CComCritSecLock<CComAutoCriticalSection>;

void SourceBuffer::buildLogTable()
{
	int x;
	for( x = 0; x < 256; x++ )
	{
		double a = log( x*60.0 / 255.0 + 1.0 ) / log( 60.0 );
		int t = (int)( a*255.0 );
		if( t < 0 )t = 0;
		if( t > 255 )t = 255;
		g_logtab[ x ] = (unsigned char)t;
	}
}

int SourceBuffer::update( struct winampVisModule *this_mod )
{
	int x, avs_beat = 0, b;

	CSLock __lock( m_cs );

	if( g_visdata_pstat )
		for( x = 0; x < 576 * 2; x++ )
			g_visdata[ 0 ][ 0 ][ x ] = g_logtab[ (unsigned char)this_mod->spectrumData[ 0 ][ x ] ];
	else
	{
		for( x = 0; x < 576 * 2; x++ )
		{
			int t = g_logtab[ (unsigned char)this_mod->spectrumData[ 0 ][ x ] ];
			if( g_visdata[ 0 ][ 0 ][ x ] < t )
				g_visdata[ 0 ][ 0 ][ x ] = t;
		}
	}
	memcpy( &g_visdata[ 1 ][ 0 ][ 0 ], this_mod->waveformData, 576 * 2 );
	{
		int lt[ 2 ] = { 0,0 };
		int x;
		int ch;
		for( ch = 0; ch < 2; ch++ )
		{
			unsigned char *f = (unsigned char*)&this_mod->waveformData[ ch ][ 0 ];
			for( x = 0; x < 576; x++ )
			{
				int r = *f++ ^ 128;
				r -= 128;
				if( r < 0 )r = -r;
				lt[ ch ] += r;
			}
		}
		lt[ 0 ] = max( lt[ 0 ], lt[ 1 ] );

		beat_peak1 = ( beat_peak1 * 125 + beat_peak2 * 3 ) / 128;

		beat_cnt++;

		if( lt[ 0 ] >= ( beat_peak1 * 34 ) / 32 && lt[ 0 ] > ( 576 * 16 ) )
		{
			if( beat_cnt > 0 )
			{
				beat_cnt = 0;
				avs_beat = 1;
			}
			beat_peak1 = ( lt[ 0 ] + beat_peak1_peak ) / 2;
			beat_peak1_peak = lt[ 0 ];
		}
		else if( lt[ 0 ] > beat_peak2 )
		{
			beat_peak2 = lt[ 0 ];
		}
		else beat_peak2 = ( beat_peak2 * 14 ) / 16;

	}
	b = refineBeat( avs_beat );
	if( b )
		g_is_beat = true;
	g_visdata_pstat = false;
	return 0;
}

void SourceBuffer::copy( char vis_data[ 2 ][ 2 ][ 576 ], bool& beat )
{
	CSLock __lock( m_cs );
	memcpy( &vis_data[ 0 ][ 0 ][ 0 ], &g_visdata[ 0 ][ 0 ][ 0 ], 576 * 2 * 2 );
	g_visdata_pstat = true;
	beat = g_is_beat;
	g_is_beat = false;
}