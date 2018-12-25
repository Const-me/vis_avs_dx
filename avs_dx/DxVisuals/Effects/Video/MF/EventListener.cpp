#include "stdafx.h"
#include "EventListener.h"

HRESULT __stdcall EventListener::GetParameters( DWORD *pdwFlags, DWORD *pdwQueue )
{
	// Implementation of this method is optional
	return E_NOTIMPL;
}

HRESULT __stdcall EventListener::Invoke( IMFAsyncResult *pAsyncResult )
{
	IMFMediaEventGenerator* gen = m_generator;
	if( nullptr == gen )
		return S_FALSE;
	CComPtr<IMFMediaEvent> me;
	CHECK( gen->EndGetEvent( pAsyncResult, &me ) );

	MediaEventType met;
	CHECK( me->GetType( &met ) );
	HRESULT hr = E_FAIL;
	CHECK( me->GetStatus( &hr ) );

	CHECK( onEvent( met, hr ) );

	if( met != METransformDrainComplete )
		CHECK( gen->BeginGetEvent( this, nullptr ) );

	return S_OK;
}

HRESULT EventListener::startListen( IMFMediaEventGenerator* eventGenerator )
{
	if( nullptr != InterlockedExchangePointer( (void**)&m_generator, eventGenerator ) )
		return HRESULT_FROM_WIN32( ERROR_ALREADY_INITIALIZED );
	CHECK( eventGenerator->BeginGetEvent( this, nullptr ) );
	return S_OK;
}

HRESULT EventListener::stopListen()
{
	return ( nullptr == InterlockedExchangePointer( (void**)&m_generator, nullptr ) ) ? S_FALSE : S_OK;
}