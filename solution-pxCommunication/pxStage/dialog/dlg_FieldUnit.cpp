#include <StdAfx.h>

#include "../../Fixture/WindowRect.h"
#include "../../DataTable/dt_FldObj.h"

#include "../resource.h"

extern char g_dir_data[];

#define _FLAGNAMESIZE 64

typedef struct
{
	short no;
	char  name[ _FLAGNAMESIZE ];

}_FLAGNAMESTRUCT;

static vector <_FLAGNAMESTRUCT> _vFlagName;

static void _Initialize( HWND hDlg )
{
//	const char* dir_table[] = {"South", "East", "Nouth", "West", "S/N"};
//	long i;

//	for( i = 0; i < 5; i++ ) SendDlgItemMessage( hDlg, IDC_COMBO_DIR, CB_ADDSTRING, 0, (LPARAM)dir_table[ i ] );

	FILE* fp = NULL;
	char  path[ MAX_PATH ];
	long  a;

	sprintf( path, "%s\\unittype.txt", g_dir_data );
	if( fp = fopen( path, "rt" ) )
	{
		while( fgets( path, MAX_PATH, fp ) )
		{
			while( 1 )
			{
				a = strlen( path ) - 1;
				if( path[ a ] != 0x0d && path[ a ] != 0x0a ) break;
				path[ a ] = '\0';
			}
			SendDlgItemMessage( hDlg, IDC_COMBO_TYPE, CB_ADDSTRING, 0, (LPARAM)path );
		}
	}

	_FLAGNAMESTRUCT flagname;

	sprintf( path, "%s\\flagnames.txt", g_dir_data );
	if( fp = fopen( path, "rt" ) )
	{
		while( fgets( path, MAX_PATH, fp ) )
		{
			if( path[ 0 ] >= '0' && path[ 0 ] <= '9' )
			{
				memset( &flagname, 0, sizeof(_FLAGNAMESTRUCT) );
				sscanf( path, "%d,%s\n", &flagname.no, flagname.name );
				_vFlagName.push_back( flagname );
				SendDlgItemMessage( hDlg, IDC_COMBO_FLAGNO, CB_ADDSTRING, 0, (LPARAM)flagname.name );
			}
		}
	}
}

static void _Release( void )
{
	_vFlagName.clear();
}

static void _SetParameter( HWND hDlg, const DT_FLDOBJ *p_eve )
{
	SendDlgItemMessage( hDlg, IDC_COMBO_TYPE, CB_SETCURSEL, p_eve->param1, 0 );
	SetDlgItemInt(      hDlg, IDC_PARAM2    , p_eve->param2, TRUE );
	SetDlgItemInt(      hDlg, IDC_X         , p_eve->x     , TRUE );
	SetDlgItemInt(      hDlg, IDC_Y         , p_eve->y     , TRUE );
}

static BOOL _GetParameter( HWND hDlg, DT_FLDOBJ *p_eve )
{
	p_eve->param1      = (unsigned char)SendDlgItemMessage( hDlg, IDC_COMBO_TYPE, CB_GETCURSEL, 0, 0 );
	p_eve->x           = GetDlgItemInt( hDlg, IDC_X        , NULL, TRUE );
	p_eve->y           = GetDlgItemInt( hDlg, IDC_Y        , NULL, TRUE );
	p_eve->param2      = GetDlgItemInt( hDlg, IDC_PARAM2   , NULL, TRUE );
	return TRUE;
}

static BOOL _CheckParameter( HWND hDlg, DT_FLDOBJ *p_eve )
{
	return TRUE;
}


//コールバック
BOOL CALLBACK dlg_FldObj( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	static DT_FLDOBJ *_p_eve;

	switch( msg ){

	//ダイアログ起動
	case WM_INITDIALOG:

		_p_eve = (DT_FLDOBJ *)l;
		_Initialize  ( hDlg );
		_SetParameter( hDlg, _p_eve );
		WindowRect_Centering( hDlg );
		break;

	case WM_CLOSE:
		break;

	//ボタンクリック
	case WM_COMMAND:

		switch(LOWORD(w))
		{
		case IDOK:
			if( _GetParameter(  hDlg, _p_eve ) )
			{
				if( _CheckParameter( hDlg, _p_eve ) )
				{
					EndDialog( hDlg, TRUE );
					_Release();
				}
			}
			break;

		case IDCANCEL:
			EndDialog( hDlg, FALSE );
			_Release();
			break;
		}
		break;


	default:return FALSE;
	
	}
	return TRUE;
}






