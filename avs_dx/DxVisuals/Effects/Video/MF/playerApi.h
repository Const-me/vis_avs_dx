#pragma once
// The huge amount of code in this folder is required to implement the following dead simple API.
// No wonder MF ain't particularly popular technology.

__interface iPlayer : IUnknown
{
	HRESULT getTexture( CComPtr<ID3D11ShaderResourceView>& srv );

	HRESULT stop();
};

HRESULT createPlayer( LPCTSTR pathToVideo, CComPtr<iPlayer>& player );