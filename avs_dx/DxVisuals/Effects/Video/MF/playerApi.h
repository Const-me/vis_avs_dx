#pragma once

__interface iPlayer : IUnknown
{
	// Open a video. Called on the GUI thread.
	HRESULT open( LPCTSTR pathToVideo );

	// Closes a video. Called on the GUI thread.
	HRESULT close();

	// Get the frame texture. Now, this one is called om the renderer thread where we can do things.
	HRESULT getTexture( CComPtr<ID3D11ShaderResourceView>& srv );
};

HRESULT createPlayer( CComPtr<iPlayer>& player );