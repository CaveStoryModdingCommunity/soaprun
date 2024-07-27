#include <StdAfx.h>

#include "../../Fixture/WindowRect.h"
#include "../../Fixture/MessageBox.h"
#include "../../DataTable/dt_Stage.h"

#include "../resource.h"
//#include "../Max.h"

#include "dlg_MapSize.h"

static void _SetParameter( HWND hDlg, const MAPSIZESTRUCT *p_size )
{
	SetDlgItemInt( hDlg, IDC_WIDTH , p_size->w, TRUE );
	SetDlgItemInt( hDlg, IDC_HEIGHT, p_size->h, TRUE );

	if( p_size->align_flags & MAPALIGNFLAG_DISABLE )
	{
		EnableWindow( GetDlgItem( hDlg, IDC_CHECK_RIGHT  ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_CHECK_BOTTOM ), FALSE );
	}
}

static BOOL _GetInputParameter( HWND hDlg, MAPSIZESTRUCT *p_size )
{
	p_size->align_flags = 0;
	p_size->w = GetDlgItemInt( hDlg, IDC_WIDTH , NULL, TRUE );
	p_size->h = GetDlgItemInt( hDlg, IDC_HEIGHT, NULL, TRUE );
	if( IsDlgButtonChecked( hDlg, IDC_CHECK_RIGHT  ) ) p_size->align_flags |= MAPALIGNFLAG_RIGHT;
	if( IsDlgButtonChecked( hDlg, IDC_CHECK_BOTTOM ) ) p_size->align_flags |= MAPALIGNFLAG_BOTTOM;

	return TRUE;
}

static BOOL _CheckParameter( HWND hDlg, MAPSIZESTRUCT *p_size )
{
	if( p_size->w < 1 || p_size->w > MAX_MAP_W ){ mbox_ERR( hDlg, "width[ 1 - %d ]",  MAX_MAP_W ); return FALSE; }
	if( p_size->h < 1 || p_size->h > MAX_MAP_L ){ mbox_ERR( hDlg, "length[ 1 - %d ]", MAX_MAP_L ); return FALSE; }
	return TRUE;
}


//コールバック
BOOL CALLBACK dlg_MapSize( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	static MAPSIZESTRUCT *_p_size;

	switch( msg ){

	//ダイアログ起動
	case WM_INITDIALOG:

		_p_size = (MAPSIZESTRUCT *)l;
		_SetParameter( hDlg, _p_size );
		WindowRect_Centering( hDlg );
		break;

	//ボタンクリック
	case WM_COMMAND:

		switch(LOWORD(w)){
		case IDOK:
			if( _GetInputParameter(  hDlg, _p_size ) ){
				if( _CheckParameter( hDlg, _p_size ) ){
					EndDialog( hDlg, TRUE );
				}
			}
			break;

		case IDCANCEL:
			EndDialog( hDlg, FALSE );
			break;
		}
		break;


	default:return FALSE;
	
	}
	return TRUE;
}






