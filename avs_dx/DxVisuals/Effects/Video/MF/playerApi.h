#pragma once

__interface iPlayer : IUnknown
{
	// Open a video. Called on the GUI thread.
	HRESULT open( LPCTSTR pathToVideo );

	// Closes a video. Called on the GUI thread.
	HRESULT close();

	// Render video to the texture. Now, this one is called om the renderer thread where we can do things.
	HRESULT render();

	ID3D11Texture2D* texture() const;
};

HRESULT createPlayer( CComPtr<iPlayer>& player );