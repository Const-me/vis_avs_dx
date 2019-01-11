#include "stdafx.h"
#include "AboutDialog.h"

BOOL AboutDialog::OnInitDialog( CWindow wndFocus, LPARAM lInitParam )
{
	ua.loadResource( IDR_GIF_UA );
	me.loadResource( IDR_GIF_ME );
	return TRUE;
}

void AboutDialog::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lp)
{
	switch( nIDCtl )
	{
	case IDC_FLAG_UA:
		ua.render( *lp );
		return;
	case IDC_FLAG_ME:
		me.render( *lp );
		return;
	}
}