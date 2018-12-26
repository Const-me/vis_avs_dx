#pragma once

__interface iPlayer : IUnknown
{
	// Open a video. Called on the GUI thread.
	HRESULT open( LPCTSTR pathToVideo );

	// Closes a video. Called on the GUI thread.
	HRESULT close();

	// Shutdown the player. Just releasing is not enough, it contains a circular COM reference due to the stupid IMFMediaEngineNotify requirement.
	void shutdown();

	// Render video to the texture. Now, this one is called on the renderer thread where we can do things. Returns S_FALSE when there's no current frame texture: no video loaded, load pending, failed, etc.
	HRESULT render();

	// Get the texture with the current video frame.
	ID3D11Texture2D* texture() const;

	// Get shader resource view of the above texture.
	ID3D11ShaderResourceView* view() const;
};

// Initialize all MF stuff and create an instance of the player.
HRESULT createPlayer( CComPtr<iPlayer>& player );