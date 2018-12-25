#pragma once
#include <mfidl.h>

// Not currently used
class MediaTypeHandler : public IMFMediaTypeHandler
{
	HRESULT __stdcall IsMediaTypeSupported( IMFMediaType *mt, IMFMediaType **ppMediaType ) override;

	HRESULT __stdcall GetMediaTypeCount( DWORD *pdwTypeCount ) override;

	HRESULT __stdcall GetMediaTypeByIndex( DWORD dwIndex, IMFMediaType **ppType ) override;

	HRESULT __stdcall SetCurrentMediaType( IMFMediaType *mt ) override;

	HRESULT __stdcall GetCurrentMediaType( IMFMediaType **ppMediaType ) override;

	HRESULT __stdcall GetMajorType( GUID *pguidMajorType ) override;

	CComPtr<IMFMediaType> m_current;

	float m_pixelAspectRatio = 1;

	void getAspect( IMFMediaType *mt );

public:

	float getPixelAspectRation() const
	{
		return m_pixelAspectRatio;
	}
};