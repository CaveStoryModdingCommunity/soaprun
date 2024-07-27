#include <StdAfx.h>

#include "resource.h"

#include "ConnectTo.h"


BOOL CALLBACK dlg_ConnectTo( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	switch( msg )
	{
	case WM_INITDIALOG:
		{
			ConnectTo cnct;
			SetDlgItemText( hDlg, IDC_IPADDRESS, cnct.ip_address     );
			SetDlgItemInt ( hDlg, IDC_PORTNO   , cnct.port_no, FALSE );
		}
		break;

	case WM_COMMAND:

		switch( LOWORD( w ) )
		{
		case IDOK    : 
			{
				ConnectTo cnct;
				GetDlgItemText( hDlg, IDC_IPADDRESS, cnct.ip_address, BUFSIZE_IPADDRESS );
				cnct.port_no = GetDlgItemInt( hDlg, IDC_PORTNO ,NULL, FALSE );
				if( cnct.Save() ){ EndDialog( hDlg, TRUE ); break; }
				MessageBox( hDlg, "Cannot Save.", "ERROR", MB_OK|MB_ICONEXCLAMATION );
				break;
			}
			break;
		case IDCANCEL:
			EndDialog( hDlg, FALSE );
			break;
		}
		break;

	default: return FALSE;
	}

	return TRUE;
}