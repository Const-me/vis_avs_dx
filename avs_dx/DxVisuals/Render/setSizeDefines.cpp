#include "stdafx.h"
#include "setSizeDefines.h"

extern int cfg_fs_d;

bool setSizeDefines( const CStringA& hlsl, Hlsl::Defines &def, iResizeHandler* pThis )
{
	bool shaderNeedsResizeEvent = false;
	if( hlsl.Find( "AVS_RENDER_SIZE" ) >= 0 )
	{
		def.set( "AVS_RENDER_SIZE", getRenderSizeString() );
		shaderNeedsResizeEvent = true;
	}
	if( hlsl.Find( "AVS_PIXEL_PARTICLES" ) >= 0 )
	{
		def.set( "AVS_PIXEL_PARTICLES", cfg_fs_d ? "1" : "0" );
		shaderNeedsResizeEvent = true;
	}

	if( shaderNeedsResizeEvent )
		subscribeHandler( pThis );
	else
		unsubscribeHandler( pThis );

	return shaderNeedsResizeEvent;
}