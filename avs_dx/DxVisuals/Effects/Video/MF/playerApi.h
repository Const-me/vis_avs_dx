#pragma once

__interface iPlayer : IUnknown
{
	HRESULT getTexture( CComPtr<ID3D11ShaderResourceView>& srv );

	HRESULT stop();
};

HRESULT createPlayer( LPCTSTR pathToVideo, CComPtr<iPlayer>& player );