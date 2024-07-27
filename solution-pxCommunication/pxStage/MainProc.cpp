
#include <StdAfx.h>

#include "../Dx/DxDraw.h"
#include "../Fixture/WindowRect.h"
#include "../Fixture/KeyControl.h"       
#include "../Fixture/MessageBox.h"

#include "../DataTable/dt_Stage.h"

#include "resource.h"

#include "interface/Interface.h"
#include "dialog/dlg_MapSize.h"


#include "StageProject.h"
#include "EditMode.h"

extern HINSTANCE g_hInst          ;
extern HMENU     g_hMenu_Main     ;
extern HWND      g_hWnd_Main      ;
extern char      g_app_name     [];
extern char      g_dir_profile  [];
extern char      g_module_path  [];
extern char      *g_main_rect_name;
extern char      *g_pcm_edit_trg  ;

extern dt_Stage  *g_stage;

static bool      _b_window_active = false;


// ウインドウ最小サイズ
extern long g_MinimizeWidth;
extern long g_MinimizeHeight;

// タイトルバー表記
void MainWindow_SetTitle( const char *path )
{
	char str[MAX_PATH];
	char *p_name;
	char *empty = "-";

	if( !path || path[0] == '\0' ) p_name = empty;
	else                           p_name = PathFindFileName( path );

#ifdef NDEBUG
	sprintf( str, "%s [%s]",       g_app_name, p_name );
#else
	sprintf( str, "%s Debug [%s]", g_app_name, p_name );
#endif
	SetWindowText( g_hWnd_Main, str );
}

static void _MinimizedWindow( void ){ _b_window_active = false; }
static void _RestoredWindow ( void ){ _b_window_active = true ; } 

// ファイルドロップ(ウインドウ)
static BOOL _Function_WM_DROPFILES( HWND hWnd, WPARAM wParam )
{
	char  path[ MAX_PATH ];
	HDROP hDrop = (HDROP)wParam;

	if( DragQueryFile( hDrop, -1, NULL, 0 ) ) DragQueryFile( hDrop,  0, path, MAX_PATH );

	DragFinish( hDrop );
	Interface_Process( TRUE ); // ?
	return TRUE;
}


//システムタスク
static BOOL _SystemTask( void )
{
	MSG msg;

	//メッセージループを生成
	while( PeekMessage(   &msg, NULL, 0, 0, PM_NOREMOVE) || !_b_window_active )
	{
		if( !GetMessage(  &msg, NULL, 0, 0 ) ) return FALSE; 
		TranslateMessage( &msg ); 
		DispatchMessage(  &msg );
	}

	return TRUE;
}

// 環境設定
static BOOL _Function_IDM_MAPSIZE( HWND hWnd )
{
	MAPSIZESTRUCT size;
	long before_w, before_l;
	long x, y;

	size.p_title = "Map Size";
	g_stage->Map_GetSize( &size.w, &size.h );
	before_w = size.w;
	before_l = size.h;
	if( DialogBoxParam( g_hInst, "DLG_MAPSIZE", hWnd, dlg_MapSize, (LPARAM)&size ) )
	{
		if( !g_stage->Map_SetSize( size.w, size.h, size.align_flags ) ) mbox_ERR( hWnd, "set map table size" );
		x = y = 0;
		if( size.align_flags & MAPALIGNFLAG_RIGHT  ) x = size.w - before_w;
		if( size.align_flags & MAPALIGNFLAG_BOTTOM ) y = size.h - before_l;
		g_stage->Unit_Shift( x, y );
	}
	return TRUE;
}

// 環境設定
static BOOL _Function_IDM_ATTRIBUTESIZE( HWND hWnd )
{
	MAPSIZESTRUCT size;
	size.p_title = "Attribute Size";
	g_stage->Attribute_GetSize( &size.w, &size.h );
	if( DialogBoxParam( g_hInst, "DLG_MAPSIZE", hWnd, dlg_MapSize, (LPARAM)&size ) )
	{
		if( !g_stage->Attribute_SetSize( size.w, size.h, size.align_flags ) ) mbox_ERR( hWnd, "set attribute table size" );
	}
	return TRUE;
}

// イベントソート
static void _Function_IDM_SORTEVENT_UP( HWND hWnd )
{
	if( MessageBox( hWnd, "sort(up)?", g_app_name, MB_OKCANCEL ) == IDOK ) g_stage->Unit_Sort_UP();
}

static void _Function_IDM_MODE( enum_EditMode mode, long map_layer )
{
	EditMode_Set( g_hMenu_Main, mode, map_layer );
}




BOOL CALLBACK dlg_NewStage  ( HWND hDlg, UINT msg, WPARAM w, LPARAM l );
BOOL CALLBACK dlg_List_Stage( HWND hDlg, UINT msg, WPARAM w, LPARAM l );

static void _IDM_STAGE_SAVE( HWND hWnd )
{
	g_stage->Stage_Save();
}

static void _IDM_STAGE_NEW( HWND hWnd )
{
	if( DialogBox( g_hInst, "DLG_NEWSTAGE", hWnd, dlg_NewStage ) )
	{
		MainWindow_SetTitle( g_stage->Stage_GetName() );
	}
}

static void _IDM_STAGE_LOAD( HWND hWnd )
{
	if( DialogBox( g_hInst, "DLG_LIST_STAGE", hWnd, dlg_List_Stage ) )
	{
		MainWindow_SetTitle( g_stage->Stage_GetName() );
	}
}


//メインウインドウプロシージャ
LRESULT CALLBACK WindowProc_Main( HWND hWnd, UINT msg, WPARAM w, LPARAM l )
{
	switch( msg)
	{
	// 初期化 ====================
	case WM_CREATE:

		g_hWnd_Main = hWnd;
		UpdateWindow( hWnd );
		MainWindow_SetTitle( NULL );

		_b_window_active = true;
		DragAcceptFiles( hWnd, TRUE );

		break;


	// 終了処理 =================
	case WM_CLOSE:

		if( g_stage->edit_IsChanged() )
		{
			switch( MessageBox( hWnd, "Overwrite Stage?", g_app_name, MB_YESNOCANCEL ) )
			{
			case IDYES   : if( !g_stage->Stage_Save() ){ mbox_ERR( hWnd, "can't save stage" ); return 0; } break;
			case IDNO    : break;
			case IDCANCEL: return 0;
			}
		}

		WindowRect_Save( hWnd, g_main_rect_name );

		DestroyWindow( hWnd );
		PostQuitMessage( 0 );
		break;


	case WM_PAINT:
		{
			HDC         hdc;
			PAINTSTRUCT ps;
	
			hdc = BeginPaint(  hWnd, &ps  );
			Interface_Process( TRUE );
			EndPaint(          hWnd, &ps  );
			break;
		}
		break;

	case WM_ACTIVATE:

		switch( LOWORD( w ) )
		{
		case WA_ACTIVE     :
		case WA_CLICKACTIVE:

			WINDOWPLACEMENT place;
			place.length = sizeof(WINDOWPLACEMENT);
			if( GetWindowPlacement( g_hWnd_Main, &place ) )
			{
				if( place.showCmd != SW_SHOWMINIMIZED ){
					Interface_Process( TRUE );
				}
			}
			_b_window_active = true;
			KeyControl_Clear();
			break;

		default:
			_b_window_active = false;
			break;
		}
		break;


	//コマンドメッセージ ========
	case WM_COMMAND:
		switch( LOWORD( w ) )
		{
		// メインウインドウを閉じさせる
		case IDM_CLOSE         : SendMessage( hWnd, WM_CLOSE, 0, 0              ); break;
		case IDM_MAPSIZE       : _Function_IDM_MAPSIZE(         hWnd            ); break;
		case IDM_ATTRIBUTESIZE : _Function_IDM_ATTRIBUTESIZE(   hWnd            ); break;
		case IDM_SORTEVENT_UP  : _Function_IDM_SORTEVENT_UP(    hWnd            ); break;
		case IDM_MODE_MAP      : _Function_IDM_MODE( enum_EditMode_Map      , 0 ); break;
		case IDM_MODE_FIELDUNIT: _Function_IDM_MODE( enum_EditMode_FldObj, 0 ); break;
//		case IDM_MODE_FIELDDOOR: _Function_IDM_MODE( enum_EditMode_FieldDoor, 0 ); break;
		case IDM_MODE_ATT1     : _Function_IDM_MODE( enum_EditMode_Attribute, 0 ); break;
		case IDM_MODE_ATT2     : _Function_IDM_MODE( enum_EditMode_Attribute, 1 ); break;
		case IDM_MODE_RESIDENT : _Function_IDM_MODE( enum_EditMode_Resident , 0 ); break;

		case IDM_STAGE_SAVE    : _IDM_STAGE_SAVE( hWnd ); break;
		case IDM_STAGE_LOAD    : _IDM_STAGE_LOAD( hWnd ); break;
		case IDM_STAGE_NEW     : _IDM_STAGE_NEW ( hWnd ); break;

/*		case IDM_ROOM:
			if( DialogBox( g_hInst, "DLG_ROOM", hWnd, dlg_Room ) )
			{
				g_stage->Room_SaveNow();
				dlg_List_Room_Update();
			}
			break;
*/
//		case IDM_SAVE_STGPRJ_AS: StageProject_SelectAndSave(    hWnd, TRUE      ); break;
//		case IDM_SAVE_STGPRJ   : StageProject_SelectAndSave(    hWnd, FALSE     ); break;
//		case IDM_LOAD_STGPRJ   : StageProject_SelectAndLoad(    hWnd            ); break;
		default:
			break;
		}
		break;

	case WM_DROPFILES: _Function_WM_DROPFILES( hWnd, w ); break;

	//サイズ変更
	case WM_SIZE:

		switch( w )
		{
		case SIZE_MINIMIZED: _MinimizedWindow(); break;
		case SIZE_MAXIMIZED:
		case SIZE_RESTORED :
			{
				WINDOWPLACEMENT place;
				place.length = sizeof(WINDOWPLACEMENT);
				if( !GetWindowPlacement( hWnd, &place ) ) return FALSE;

				RECT rc;
				GetClientRect(  hWnd, &rc );
				InvalidateRect( hWnd, &rc, FALSE );
				UpdateWindow(   hWnd );
				_RestoredWindow();
			}
			break;
		}

		break;

	// 最小サイズ
	case WM_GETMINMAXINFO:
		{
			RECT rcWorkArea;
			SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 );

			MINMAXINFO* lpMMI = (MINMAXINFO*)l;
			lpMMI->ptMinTrackSize.x = g_MinimizeWidth;   // 最小幅
			lpMMI->ptMinTrackSize.y = g_MinimizeHeight;  // 最小高
			lpMMI->ptMaxTrackSize.x = rcWorkArea.right;  // 最大幅を作業領域のサイズ
			lpMMI->ptMaxTrackSize.y = rcWorkArea.bottom; // 最大高を作業領域のサイズ
		}
		break;

	case WM_KEYDOWN    :
	case WM_KEYUP      :
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP  :
	case WM_RBUTTONUP  :
		KeyControl_WM_MESSAGE( hWnd, msg, w );
		Interface_Process( FALSE );
		break;

	case WM_MOUSEMOVE:
		Interface_Process( TRUE );
		break;

	case WM_MOUSEWHEEL:
		KeyControl_WM_MOUSEWHEEL( hWnd, w );
		Interface_Process( TRUE );
		break;

	default://上記スイッチ記述のないものはWindowsに処理を委ねる
		return DefWindowProc( hWnd, msg, w, l );
	}

    return 0;
}
