#pragma once

using pfnExtensionFilter = bool( *)( const CString& ext );

int initAssetsCombobox( HWND wndDialog, HWND wndComboBox, char *selectedName, pfnExtensionFilter extFilter );

HRESULT getAssetFilePath( const char *selection, pfnExtensionFilter extFilter, CString& result );