#include "stdafx.h"
#include "MediaTypeHandler.h"
#include "mf.h"

HRESULT __stdcall MediaTypeHandler::GetMajorType( GUID *pguidMajorType )
{
	*pguidMajorType = MFMediaType_Video;
	return S_OK;
}

HRESULT __stdcall MediaTypeHandler::GetMediaTypeCount( DWORD *pdwTypeCount )
{
	*pdwTypeCount = m_current ? 1 : 0;
	return S_OK;
}

HRESULT __stdcall MediaTypeHandler::GetMediaTypeByIndex( DWORD dwIndex, IMFMediaType **ppType )
{
	if( 0 != dwIndex )
		return E_INVALIDARG;

	CComPtr<IMFMediaType> current = m_current;
	if( current )
	{
		*ppType = current.Detach();
		return S_OK;
	}
	return MF_E_NOT_INITIALIZED;
}

HRESULT __stdcall MediaTypeHandler::SetCurrentMediaType( IMFMediaType *mt )
{
	dbgLogMediaType( "SetCurrentMediaType", mt );
	if( nullptr != mt )
		getAspect( mt );
	m_current = mt;
	return S_OK;
}

HRESULT __stdcall MediaTypeHandler::GetCurrentMediaType( IMFMediaType **ppMediaType )
{
	if( !m_current )
	{
		// return MF_E_NOT_INITIALIZED;
		CHECK( MFCreateMediaType( &m_current ) );
		CHECK( m_current->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Video ) );
		CHECK( m_current->SetGUID( MF_MT_SUBTYPE, MFVideoFormat_RGB32 ) );
		CHECK( m_current->SetUINT32( MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive ) );
		CHECK( m_current->SetUINT32( MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE ) );
	}

	CComPtr<IMFMediaType> current = m_current;
	*ppMediaType = current.Detach();
	return S_OK;
}

static HRESULT isSupported( IMFMediaType *mt )
{
	GUID minor;
	CHECK( mt->GetGUID( MF_MT_SUBTYPE, &minor ) );

	if( minor != MFVideoFormat_RGB32 )
		return MF_E_INVALIDMEDIATYPE;

	// return S_OK;

	MFVideoInterlaceMode interlace;
	CHECK( mt->GetUINT32( MF_MT_INTERLACE_MODE, (UINT32*)&interlace ) );

	if( interlace != MFVideoInterlace_Progressive )
		return MF_E_INVALIDMEDIATYPE;

	return S_OK;
}

void MediaTypeHandler::getAspect( IMFMediaType *mt )
{
	UINT32 num, den;
	if( FAILED( MFGetAttributeRatio( mt, MF_MT_PIXEL_ASPECT_RATIO, &num, &den ) ) )
		m_pixelAspectRatio = 1;
	else
		m_pixelAspectRatio = (float)num / (float)den;
}

HRESULT __stdcall MediaTypeHandler::IsMediaTypeSupported( IMFMediaType *mt, IMFMediaType **ppMediaType )
{
	GUID major;
	CHECK( mt->GetMajorType( &major ) );
	if( major != MFMediaType_Video )
		return MF_E_INVALIDMEDIATYPE;

	if( S_OK == isSupported( mt ) )
	{
		getAspect( mt );
		if( nullptr != ppMediaType )
			*ppMediaType = nullptr;
		// Completely undocumented, but apparently the framework expects me to set the current one to the requested.

		m_current = nullptr;;
		CHECK( MFCreateMediaType( &m_current ) );
		CHECK( mt->CopyAllItems( m_current ) );

		dbgLogMediaType( "IsMediaTypeSupported: YES", mt );
		return S_OK;
	}

	if( nullptr == ppMediaType )
	{
		dbgLogMediaType( "IsMediaTypeSupported: NO", mt );
		return MF_E_INVALIDMEDIATYPE;
	}

	CComPtr<IMFMediaType> closest;
	CHECK( MFCreateMediaType( &closest ) );
	CHECK( mt->CopyAllItems( closest ) );
	CHECK( closest->SetGUID( MF_MT_SUBTYPE, MFVideoFormat_RGB32 ) );
	CHECK( closest->SetUINT32( MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive ) );

	dbgLogMediaType( "IsMediaTypeSupported: NO, offered another one", mt );
	return MF_E_INVALIDMEDIATYPE;
}