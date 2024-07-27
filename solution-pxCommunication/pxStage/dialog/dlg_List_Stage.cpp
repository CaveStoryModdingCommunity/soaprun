#include <StdAfx.h>

#include "../../Fixture/WindowRect.h"
#include "../../Fixture/MessageBox.h"
#include "../../DataTable/dt_Stage.h"

//#include "../interface/Interface.h"
#include "../StageProject.h"

#include "../resource.h"

//#include "dlg_List_Room.h"

extern char      g_dir_data[];
extern dt_Stage *g_stage;

static char* _rect_name = "list_stage.rect";
static HWND  _hList     = NULL;
static BOOL  _b_Ret = FALSE;
static vector <char*> _v_name;

enum
{
	_COLUMN_NAME,
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
	for( unsigned int i = 0; i < _v_name.size(); i++ ) free( _v_name.at( i ) );
	_v_name.clear();
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

static bool _dlg_List_Stage_Init( HWND hDlg )
{
	DWORD     flags;

	_b_Ret = FALSE;

	_hList = GetDlgItem( hDlg, IDC_LIST );
	flags = ListView_GetExtendedListViewStyle( _hList );
	ListView_SetExtendedListViewStyle( _hList, flags | LVS_EX_FULLROWSELECT );

	_ListView_AddColumn( LVCFMT_LEFT , 120, "Name" , _COLUMN_NAME       );

	// g_dir_data ‚ðŒŸõ
	WIN32_FIND_DATA find ;
	HANDLE          hFind = INVALID_HANDLE_VALUE;
	char            path[ MAX_PATH ];
	int             len;
	char*           p;
	LV_ITEM         item;

	sprintf( path, "%s\\*.%s", g_dir_data, g_stage->Ext() );
	if( ( hFind = FindFirstFile( path, &find ) ) != INVALID_HANDLE_VALUE )
	{
		do
		{
			memset( &item, 0, sizeof(LV_ITEM) );

			len = strlen( find.cFileName ) - strlen( g_stage->Ext() );
			p = (char*)malloc( len );
			memcpy( p, find.cFileName, len );
			p[ len - 1 ] = '\0';
			_v_name.push_back( p );

			item.mask     = LVIF_TEXT;
			item.iSubItem = _COLUMN_NAME;
			item.pszText  = p;
			item.iItem    = ListView_GetItemCount( _hList );
			ListView_InsertItem( _hList, &item );
		}
		while( FindNextFile( hFind, &find ) );
		if( hFind != INVALID_HANDLE_VALUE ) FindClose( hFind );
	}

	if( !WindowRect_Load( hDlg, _rect_name, TRUE ) ) WindowRect_Centering( hDlg );
	return true;
}


static void _NM_DBLCLK( HWND hDlg )
{
	int v = ListView_GetNextItem( _hList, -1, LVNI_ALL | LVNI_SELECTED );
	if( v == -1 ) return;

	StageProject_Load2( hDlg, _v_name.at( v ) );
/*
	int err_flags = g_stage->Stage_Load( _v_name.at( v ) );

	if( err_flags & ERRFLAG_ATTRIBUTE       ) mbox_ERR( hDlg, "Err: Load Attribute"          );
	if( err_flags & ERRFLAG_MAPPARTSIMAGE   ) mbox_ERR( hDlg, "Err: Load Map-Parts-Image"    );
	if( err_flags & ERRFLAG_MAP             ) mbox_ERR( hDlg, "Err: Load Map-Binary"         );
	if( err_flags & ERRFLAG_UNIT            ) mbox_ERR( hDlg, "Err: Load Unit-Binary"        );
	if( 0&&err_flags & ERRFLAG_lCASTIMAGE   ) mbox_ERR( hDlg, "Err: Load Unit-lCast-Image"   );
	if( 0&&err_flags & ERRFLAG_lSYMBOLIMAGE ) mbox_ERR( hDlg, "Err: Load Unit-lSymbol-Image" );

	Interface_Process( TRUE );
*/
	_b_Ret = TRUE;
	PostMessage( hDlg, WM_CLOSE, 0, 0 );
}

BOOL CALLBACK dlg_List_Stage( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	switch( msg )
	{
	case WM_INITDIALOG:

		_dlg_List_Stage_Init( hDlg );
		break;

	case WM_CLOSE:
		_ListView_Clear();
		WindowRect_Save( hDlg, _rect_name );
		EndDialog( hDlg, _b_Ret );
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
