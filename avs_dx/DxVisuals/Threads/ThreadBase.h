#pragma once
struct winampVisModule;

// Base class for the implementation of a thread
class ThreadBase
{
protected:
	friend class AvsThreads;

	winampVisModule *const this_mod;

	ThreadBase() = delete;
	ThreadBase( winampVisModule * pModule ) :
		this_mod( pModule ), threadId( GetCurrentThreadId() ) { }

	virtual HRESULT initialize() = 0;

	virtual HRESULT run() = 0;

public:

	const uint32_t threadId;

	virtual void postQuitmessage() = 0;
};