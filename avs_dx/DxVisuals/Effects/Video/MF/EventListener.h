#pragma once
#include <mfobjects.h>

class EventListener: public IMFAsyncCallback
{
	HRESULT __stdcall GetParameters( DWORD *pdwFlags, DWORD *pdwQueue ) override;

	HRESULT __stdcall Invoke( IMFAsyncResult *pAsyncResult ) override;

	IMFMediaEventGenerator* volatile m_generator = nullptr;

protected:

	HRESULT startListen( IMFMediaEventGenerator* eventGenerator );

	HRESULT stopListen();

	virtual HRESULT onEvent( MediaEventType eventType ) = 0;
};