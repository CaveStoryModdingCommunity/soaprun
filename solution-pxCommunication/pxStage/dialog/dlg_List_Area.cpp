#include <StdAfx.h>

#include "../../Fixture/WindowRect.h"
#include "../../Fixture/MessageBox.h"
#include "../../DataTable/dt_Stage.h"

#include "../resource.h"
#include "../NameList.h"

#include "../interface/Interface.h"

#include "dlg_List_Room.h"

extern char      g_app_name[];
extern HINSTANCE g_hInst;
extern char      g_dir_data[];
extern dt_Stage  *g_stage    ;
extern NameList  *g_namelist_area;

static char* _rect_name = "list_area.rect";
static HWND  _hList     = NULL;

static vector <char*> _v_name;

BOOL CALLBACK dlg_FieldArea( HWND hDlg, UINT msg, WPARAM w, LPARAM l );

enum
{
	_COLUMN_AREANO,
	_COLUMN_NAME  ,
};

static void _ListView_Resize( HWND hDlg )
{
	if( _hList )
	{
		RECT rc;
		GetClientRect( hDlg, &rc );
		SetWindowPos( _hList, NULL, rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER );
	}
}

static void _ListView_Clear( void )
{
	// clear
	ListView_DeleteAllItems( _hList );
//	for( unsigned int i = 0; i < _v_name.size(); i++ ) free( _v_name.at( i ) );
//	_v_name.clear();
}

static void _ListView_AddColumn( int fmt, int cx, LPSTR text, int i )
{
	LV_COLUMN clmn;

	memset( &clmn, 0, sizeof(LV_COLUMN) );

	clmn.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	clmn.fmt      = fmt ;
	clmn.cx       = cx  ;
	clmn.pszText  = text;
	clmn.iSubItem = i   ;

	ListView_InsertColumn( _hList, i, &clmn );
}

void dlg_List_Area_Update()
{
	_ListView_Clear();

	// g_dir_data ‚ðŒŸõ
	LV_ITEM item     ;
	char    str[ 64 ];

	memset( &item, 0, sizeof(LV_ITEM) );
	item.mask     = LVIF_TEXT;
	item.pszText  = str;

	const DT_FIELDAREASTRUCT* p_area;

	for( int i = 0; p_area = g_stage->Area_edit_Get_byIndex( i ); i++ )
	{
		item.iItem    = i;

		item.iSubItem = _COLUMN_AREANO;
		sprintf( str, "%02d", p_area->no   );
		ListView_InsertItem( _hList, &item );

		item.iSubItem = _COLUMN_NAME  ;
		sprintf( str, "%s", g_namelist_area->GetName_byNo( p_area->no ) );
		ListView_SetItem   ( _hList, &item );
	}
}

static bool _dlg_List_Area_Init( HWND hDlg )
{
	DWORD flags;

	_hList = GetDlgItem( hDlg, IDC_LIST );
	flags  = ListView_GetExtendedListViewStyle( _hList );
	ListView_SetExtendedListViewStyle( _hList, flags | LVS_EX_FULLROWSELECT );

	_ListView_AddColumn( LVCFMT_RIGHT,  40, "No"  , _COLUMN_AREANO );
	_ListView_AddColumn( LVCFMT_LEFT ,  80, "Name", _COLUMN_NAME   );

	if( !WindowRect_Load( hDlg, _rect_name, TRUE ) ) WindowRect_Centering( hDlg );
	return true;
}


static void _NM_DBLCLK( HWND hDlg )
{
	int v = ListView_GetNextItem( _hList, -1, LVNI_ALL | LVNI_SELECTED );

	static DT_FIELDAREASTRUCT area;

	// Change!
	if( v != -1 )
	{
		const DT_FIELDAREASTRUCT *p_area = g_stage->Area_edit_Get_byIndex( v );
		if( !p_area ) return;
		memcpy( &area, p_area, sizeof(DT_FIELDAREASTRUCT) );
		if( !DialogBoxParam( g_hInst, "DLG_FIELDAREA", hDlg, dlg_FieldArea, (LPARAM)&area ) ) return;
		g_stage->Area_edit_Set_byIndex( v, &area );
		dlg_List_Area_Update ();
		Interface_Process( TRUE );
	}
	// New!
	else
	{
		memset( &area, 0, sizeof(DT_FIELDAREASTRUCT) );
		area.flags |= AREARECORD_FLAG_VALID;
		if( !DialogBoxParam( g_hInst, "DLG_FIELDAREA", hDlg, dlg_FieldArea, (LPARAM)&area ) ) return;
		if( !g_stage->Area_edit_Add( &area ) ) return;
		dlg_List_Area_Update ();
		Interface_Process( TRUE );
	}
	
}


BOOL CALLBACK dlg_List_Area( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	switch( msg )
	{
	case WM_INITDIALOG:

		_dlg_List_Area_Init( hDlg );
		break;

	case WM_DESTROY:
		_ListView_Clear();
		WindowRect_Save( hDlg, _rect_name );
		break;

	case WM_SIZE:

		switch( w )
		{
		case SIZE_MAXIMIZED:
		case SIZE_MINIMIZED:
		case SIZE_RESTORED : _ListView_Resize( hDlg ); break;
		}
		break;

	case WM_NOTIFY:

		if( (int)w == IDC_LIST )
		{
			LPNMHDR pNtfy = (LPNMHDR)l;
			if( pNtfy->code == NM_DBLCLK ) _NM_DBLCLK( hDlg );
        }
        break;

	default: return FALSE;
	}

	return TRUE;
}
