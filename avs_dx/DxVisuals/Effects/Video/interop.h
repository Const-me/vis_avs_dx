#pragma once
class iRootEffect;

// Search for suitable video files under g_path, "C:\Soft\Winamp\Plugins\avs" on my PC, add them to the combobox.
int initVideoCombobox( HWND wndDialog, HWND wndComboBox, char *selectedName );

// Start playing the video
HRESULT videoOpen( iRootEffect* pEffect, const char *selection );

// Close the video
HRESULT videoClose( iRootEffect* pEffect );