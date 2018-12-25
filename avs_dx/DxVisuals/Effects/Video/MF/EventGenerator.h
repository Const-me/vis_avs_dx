#pragma once
#include "mf.h"

// Implements IMFMediaEventGenerator portion of IMFStreamSink interface.
class EventGenerator : public IMFStreamSink
{
public:

	bool wasShutdown() const;

protected:

	HRESULT startup();

	void shutdown();

private:

	CComPtr<IMFMediaEventQueue> m_queue;
	std::atomic_bool m_shutdown = false;

	// ==== IMFMediaEventGenerator methods ====

	HRESULT __stdcall GetEvent( DWORD dwFlags, IMFMediaEvent **ppEvent ) override;

	HRESULT __stdcall BeginGetEvent( IMFAsyncCallback *pCallback, IUnknown *punkState ) override;

	HRESULT __stdcall EndGetEvent( IMFAsyncResult *pResult, IMFMediaEvent **ppEvent ) override;

protected:

	HRESULT __stdcall QueueEvent( MediaEventType met, const GUID &guidExtendedType = GUID_NULL, HRESULT hrStatus = S_OK, const PROPVARIANT *pvValue = nullptr ) override;
};