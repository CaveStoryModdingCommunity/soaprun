#include <StdAfx.h>

#include "../../Fixture/WindowRect.h"
#include "../../Fixture/MessageBox.h"

#include "../../DataTable/dt_Stage.h"

#include "../resource.h"
#include "../NameList.h"

extern NameList *g_namelist_area;

static char     *_rect_name = "area.rect";

static int _idc_enegroups[ MAX_AREAENEGROUPCODE ] =
{
	IDC_ENEMY_GROUP_0,
	IDC_ENEMY_GROUP_1,
	IDC_ENEMY_GROUP_2,
	IDC_ENEMY_GROUP_3,
	IDC_ENEMY_GROUP_4,
};


void _Initialize_dlg_FieldArea( HWND hDlg, const DT_FIELDAREASTRUCT *p_area )
{
	const NAMERECORD *p_namerec;
	char  str[ 100 ];

	for( int r = 0; p_namerec = g_namelist_area->GetRecord_byIndex( r ); r++ )
	{
		if( !p_namerec->no ) break;
		sprintf( str, "%02d %s", p_namerec->no, p_namerec->p_name );
		SendDlgItemMessage( hDlg, IDC_COMBO_AREANO, CB_ADDSTRING, 0, (LPARAM)str ); 
	}

	SendDlgItemMessage( hDlg, IDC_COMBO_AREANO, CB_SETCURSEL, g_namelist_area->GetIndex_byNo( p_area->no ), 0 );

	SetDlgItemInt( hDlg, IDC_ENCOUNTER, p_area->encounter, FALSE );
	
	for( int i = 0; i < MAX_AREAENEGROUPCODE; i++ )
	{
		if( p_area->enegroup_codes[ i ] ) SetDlgItemInt( hDlg, _idc_enegroups[ i ], p_area->enegroup_codes[ i ], FALSE );
	}

}

bool _Apply_FieldArea( HWND hDlg, DT_FIELDAREASTRUCT *p_area )
{
	int index = SendDlgItemMessage( hDlg, IDC_COMBO_AREANO, CB_GETCURSEL, 0, 0 );

	p_area->no        = g_namelist_area->GetRecord_byIndex( index )->no;
	p_area->encounter = (unsigned char)GetDlgItemInt( hDlg, IDC_ENCOUNTER, NULL, FALSE );

	int code_num = 0;

	for( int i = 0; i < MAX_AREAENEGROUPCODE; i++ )
	{
		int code = GetDlgItemInt( hDlg, _idc_enegroups[ i ], NULL, FALSE );
		if( code ) p_area->enegroup_codes[ code_num++ ] = code;
	}

	for( int i = code_num; i < MAX_AREAENEGROUPCODE; i++ ) p_area->enegroup_codes[ i ] = 0;

	return true;
}

BOOL CALLBACK dlg_FieldArea( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	static DT_FIELDAREASTRUCT *_p_area;

	switch( msg )
	{
	case WM_INITDIALOG:

		_p_area = (DT_FIELDAREASTRUCT *)l;
		if( !WindowRect_Load( hDlg, _rect_name, FALSE ) ) WindowRect_Centering( hDlg );
		_Initialize_dlg_FieldArea( hDlg, _p_area );

		break;

	case WM_CLOSE:
		WindowRect_Save( hDlg, _rect_name );
		EndDialog( hDlg, TRUE );
		break;

	case WM_COMMAND:

		switch( LOWORD(w) )
		{
		case IDOK:
			if( !_Apply_FieldArea( hDlg, _p_area ) ) break;
			EndDialog( hDlg, TRUE  );
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