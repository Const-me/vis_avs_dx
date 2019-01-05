#pragma once

// Search for suitable video files under g_path, "C:\Soft\Winamp\Plugins\avs" on my PC, add them to the combobox.
int initPictureCombobox( HWND wndDialog, HWND wndComboBox, char *selectedName );

// Start playing the video
HRESULT pictureOpen( const C_RBASE* pEffect, const char *selection );

// Close the video
HRESULT pictureClose( const C_RBASE* pEffect );