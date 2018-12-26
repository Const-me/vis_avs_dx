#pragma once
#include "playerApi.h"
#include "mf.h"
#include "FrameTexture.h"

class Player:
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMFMediaEngineNotify,
	public iPlayer
{
	CComAutoCriticalSection m_cs;
	CComBSTR m_path;
	bool m_updated;

	CComPtr<IMFMediaEngine> m_engine;
	FrameTexture m_texture;

	HRESULT ensureEngine();

public:

	BEGIN_COM_MAP( Player )
		COM_INTERFACE_ENTRY( IMFMediaEngineNotify )
	END_COM_MAP()

	~Player();

private:

	HRESULT open( LPCTSTR pathToVideo ) override;

	HRESULT close() override;

	ID3D11Texture2D* texture() const override
	{
		return m_texture.texture();
	}

	ID3D11ShaderResourceView* view() const override
	{
		return m_texture.view();
	}

	HRESULT render() override;

	void shutdown() override;

	// IMFMediaEngineNotify
	HRESULT __stdcall EventNotify( DWORD e, DWORD_PTR param1, DWORD param2 ) override;
};