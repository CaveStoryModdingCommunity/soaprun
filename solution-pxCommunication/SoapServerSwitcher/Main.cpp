#include <windows.h>
#include <stdio.h>

#include "../Fixture/AppDirectory.h"
#include "../Fixture/WindowRect.h"
#include "../Fixture/MessageBox.h"

#include "../ws/ws.h"

//#include "../SoapCGI/ServerInformation.h"
#include "../SoapCGI/CGI.h"

#include "resource.h"

char       *g_dir_profile = NULL ;

static const char *_rect_name = "switcher.rect" ;
static const char *_filename  = "serverinfo.bin";

static DWORD _idc_tbl_comment[ SERVERCOMMENT_NUM ] =
{
	IDC_COMMENT_0,
	IDC_COMMENT_1,
	IDC_COMMENT_2,
	IDC_COMMENT_3,
	IDC_COMMENT_4,
	IDC_COMMENT_5
};

static void _WM_INITDIALOG( HWND hDlg )
{
	if( !WindowRect_Load( hDlg, _rect_name, FALSE ) ) WindowRect_Centering( hDlg );

	SERVERINFORMATION sv;
	char path[ MAX_PATH ];
	sprintf( path, "%s\\%s", g_dir_profile, _filename );
	ServerInformation_Load( path, &sv );

	SetDlgItemText( hDlg, IDC_IPADDRESS, sv.ip_address     );
	SetDlgItemText( hDlg, IDC_PROTOCOL , sv.protocol       );
	for( int c = 0; c < SERVERCOMMENT_NUM; c++ )
	{
		SetDlgItemText( hDlg, _idc_tbl_comment[ c ], sv.comments[ c ] );
	}

	SetDlgItemInt ( hDlg, IDC_PORTNO   , sv.port_no, FALSE );
	SetDlgItemInt ( hDlg, IDC_VERSION  , sv.version, FALSE );

	switch( sv.status )
	{
	case SERVERSTATUS_Open: CheckDlgButton( hDlg, IDC_CHECK_OPEN, 1 ); break;
	default               : CheckDlgButton( hDlg, IDC_CHECK_OPEN, 0 ); break;
	}
}

static void _WM_CLOSE( HWND hDlg )
{
	WindowRect_Save( hDlg, _rect_name );
	EndDialog( hDlg, TRUE );
}


static void _IDOK( HWND hDlg )
{
	SERVERINFORMATION sv;
	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s", g_dir_profile, _filename );
	ServerInformation_Load( path, &sv );

	GetDlgItemText( hDlg, IDC_IPADDRESS, sv.ip_address, BUFFERSIZE_IPADDRESS     );
	GetDlgItemText( hDlg, IDC_PROTOCOL , sv.protocol  , BUFFERSIZE_PROTOCOL      );

	for( int c = 0; c < SERVERCOMMENT_NUM; c++ )
	{
		GetDlgItemText( hDlg, _idc_tbl_comment[ c ], sv.comments[ c ], BUFFERSIZE_SERVERCOMMENT );
	}

	sv.port_no = GetDlgItemInt( hDlg, IDC_PORTNO , NULL, FALSE );
	sv.version = GetDlgItemInt( hDlg, IDC_VERSION, NULL, FALSE );

	sv.status  = IsDlgButtonChecked( hDlg, IDC_CHECK_OPEN ) ? SERVERSTATUS_Open : SERVERSTATUS_Close;

	if( !ServerInformation_Save( path, &sv ) )
	{
		mbox_ERR( hDlg, "Can't save.." );
		return;
	}

//	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	bool b_stop = false;

	if( !CGI_SetServerInformation( &sv, &b_stop ) )
	{
		mbox_ERR( hDlg, "Can't server.." );
		return;
	}
	MessageBox( hDlg, "ok.", "", MB_OK );

//	WindowRect_Save( hDlg, _rect_name );
//	EndDialog( hDlg, TRUE );
}

BOOL CALLBACK _Procedure( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	switch( msg )
	{
	case WM_INITDIALOG: _WM_INITDIALOG( hDlg ); break;
	case WM_CLOSE     : _WM_CLOSE     ( hDlg ); break;
	case WM_COMMAND   :

		switch( LOWORD(w) )
		{
		case IDCANCEL: _WM_CLOSE  ( hDlg ); break;
		case IDOK    : _IDOK    ( hDlg ); break;
		}
		break;

	default: return FALSE;
	}
	return TRUE ;
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR pCmd, int nCmd )
{
	AppDirectory_Ready   ( &g_dir_profile, "profile" );
	WindowRect_Initialize(  g_dir_profile );

	ws_Initialize ();
	CGI_Initialize();

	DialogBox( hInst, "DLG_MAIN", NULL, _Procedure );

	CGI_Release         (                );
	ws_Release          (                );
	AppDirectory_Release( &g_dir_profile );
	return 0;
}
