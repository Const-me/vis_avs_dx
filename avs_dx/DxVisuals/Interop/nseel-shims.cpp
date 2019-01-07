#include "stdafx.h"
#include "../../avs/vis_avs/avs_eelif.h"

char last_error_string[ 1024 ];
int g_log_errors;
CRITICAL_SECTION g_eval_cs;

void AVS_EEL_IF_init()
{
	InitializeCriticalSection( &g_eval_cs );
}

void AVS_EEL_IF_quit()
{
	DeleteCriticalSection( &g_eval_cs );
}

int AVS_EEL_IF_Compile( int context, char *code ) { return 0; }
void AVS_EEL_IF_Execute( void *handle, char visdata[ 2 ][ 2 ][ 576 ] ) { }
void AVS_EEL_IF_resetvars( NSEEL_VMCTX ctx ) { }
void AVS_EEL_IF_VM_free( NSEEL_VMCTX ctx ) { }

static array<int, 5> nseel_evallib_stats = {}; // source bytes, static code bytes, call code bytes, data bytes, segments
static array<double, 100> nseel_globalregs = {};

extern "C"
{
	int *NSEEL_getstats()
	{
		return nseel_evallib_stats.data();
	}

	double *NSEEL_getglobalregs()
	{
		return nseel_globalregs.data();
	}

	NSEEL_VMCTX NSEEL_VM_alloc() // return a handle
	{
		size_t res = 0;
		res = ~res;
		return (NSEEL_VMCTX)res;
	}
	double *NSEEL_VM_regvar( NSEEL_VMCTX _ctx, char *var )
	{
		return &nseel_globalregs[ 0 ];
	}
	void NSEEL_code_free( NSEEL_CODEHANDLE code )
	{
	}
}