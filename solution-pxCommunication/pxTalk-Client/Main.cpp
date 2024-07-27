#include <StdAfx.h>

#include "../ws/wsClient.h"

#include "../Fixture/AppDirectory.h"
#include "../Fixture/AppExisting.h"
#include "../Fixture/WindowRect.h"
#include "../Fixture/DebugLog.h"
#include "../Fixture/ShiftJIS.h"
#include "../Fixture/MessageBox.h"

#include "../ws/ws.h"

BOOL CALLBACK dlg_ConnectTo( HWND hDlg, UINT msg, WPARAM w, LPARAM l );
BOOL CALLBACK dlg_Table    ( HWND hDlg, UINT msg, WPARAM w, LPARAM l );


int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR pCmd, int nCmd )
{
	g_hInst = hInst;

#ifdef NDEBUG

	AppExisting app_ext;

	static char *mutex_name   = "pxtalkclient"    ;
	static char *mapping_name = "map_pxtalkclient";

	if( app_ext.IsExisting( mutex_name, mapping_name ) )
	{
		MessageBox( NULL, "Ç∑Ç≈Ç…ãNìÆÇµÇƒÇ¢Ç‹Ç∑ÅB", g_app_name, MB_OK|MB_ICONEXCLAMATION );
		return 1;
	}

	app_ext.Mapping( NULL, mutex_name, mapping_name );

#endif

	InitCommonControls();
	timeBeginPeriod( 1 );

	AppDirectory_Check   ( &g_dir_data   , "data"    );
	AppDirectory_Ready   ( &g_dir_profile, "profile" );
	WindowRect_Initialize(  g_dir_profile );
	DebugLog_Initialize  (  g_dir_profile );
	DebugLog_DeleteOlds  ( 10 );

	dlog( "#Start[ %s ]", g_app_name );


	ws_Initialize ();


	if( DialogBox( hInst, "DLG_CONNECTTO", NULL, dlg_ConnectTo ) )
	{
		wsClient client;
		DialogBoxParam( hInst, "DLG_TABLE", NULL, dlg_Table, (LPARAM)&client );
	}


	ws_Release (); dlog( "released: ws"  );

	dlog( "End." );

	DebugLog_Release();
	AppDirectory_Release( &g_dir_profile );

	timeEndPeriod  ( 1 );

	return 1;
}
