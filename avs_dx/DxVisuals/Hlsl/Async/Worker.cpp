#include "stdafx.h"
#include "Worker.h"

using namespace Hlsl;

Worker::Worker()
{
	m_work = CreateThreadpoolWork( &Worker::callbackStatic, this, nullptr );
	assert( nullptr != m_work );
}

Worker::~Worker()
{
	shutdown();
}

void Worker::shutdown()
{
	if( m_work )
	{
		WaitForThreadpoolWorkCallbacks( m_work, TRUE );
		CloseThreadpoolWork( m_work );
		m_work = nullptr;
	}
}

void Worker::launch()
{
	assert( m_work );
	SubmitThreadpoolWork( m_work );
}

void Worker::join()
{
	if( m_work )
		WaitForThreadpoolWorkCallbacks( m_work, FALSE );
}

void __stdcall Worker::callbackStatic( TP_CALLBACK_INSTANCE *Instance, void* pv, PTP_WORK Work )
{
	// CallbackMayRunLong( Instance );
	Worker* pWorker = (Worker*)pv;
	pWorker->workCallback();
}