#pragma once
#include <../../avs/vis_avs/resource.h>
#include "Utils/Picture.h"
#include "Utils/WTL/atlapp.h"
#include "Utils/WTL/atlcrack.h"

class AboutDialog: public CDialogImpl<AboutDialog>
{
	Picture ua, me;
public:
	enum { IDD = IDD_ABOUT_DX };

	BEGIN_MSG_MAP( AboutDialog )
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_DRAWITEM( OnDrawItem )
		COMMAND_ID_HANDLER( IDOK, OnCloseCmd )
		COMMAND_ID_HANDLER( IDCANCEL, OnCloseCmd )
	END_MSG_MAP()

private:

	BOOL OnInitDialog( CWindow wndFocus, LPARAM lInitParam );
	void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );

	LRESULT OnCloseCmd( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
	{
		EndDialog( wID );
		return 0;
	}
};