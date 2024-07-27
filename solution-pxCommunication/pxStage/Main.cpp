#include <StdAfx.h>

#include "../Dx/DxDraw.h"                  
#include "../Fixture/WindowRect.h"         
#include "../Fixture/MessageBox.h" 
#include "../Fixture/KeyControl.h"   


#include "../DataTable/dt_Stage.h"

#include "interface/Interface.h"
#include "interface/if_Generic.h"
#include "interface/SurfaceNo.h"

#include "resource.h"

#include "EditMode.h"
#include "StageProject.h"

HINSTANCE    g_hInst;
HWND         g_hWnd_Main  = NULL;
//HWND         g_hList_Room = NULL;
HMENU        g_hMenu_Main = NULL;

dt_Stage     *g_stage    ;

char         g_module_path[ MAX_PATH ];
char         g_dir_profile[ MAX_PATH ];
char         g_dir_data   [ MAX_PATH ];
char         g_app_name    [    32    ];
		   
char         *g_main_rect_name = "main.rect"   ;
char         *g_pcm_edit_trg   = "edit.trigger";
static char  *_class_name      = "Main"        ;
static long  _mag              =              1;
		   
long         g_MinimizeWidth;
long         g_MinimizeHeight;

void             MainWindow_SetTitle( const char *path );
LRESULT CALLBACK WindowProc_Main( HWND hWnd, UINT msg, WPARAM w, LPARAM l );
//BOOL    CALLBACK dlg_List_Room  ( HWND hDlg, UINT msg, WPARAM w, LPARAM l );
BOOL    Function_IDM_PCM_EDIT   ( HWND hWnd );


//////////////////////////////////////////
// ローカル関数 //////////////////////////
//////////////////////////////////////////

// ウインドウクラスの登録
static BOOL _RegistWindowClass( HINSTANCE hInst, char *class_name, WNDPROC lpfnWndProc )
{

    WNDCLASSEX wc;

	memset( &wc, 0, sizeof(WNDCLASSEX) );

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpszClassName = class_name;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = lpfnWndProc;
    wc.hInstance     = hInst;        //インスタンス
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);//(HBRUSH)(COLOR_APPWORKSPACE + 1);
    wc.hIcon         = LoadIcon( hInst, "0" );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW       );

    if( !RegisterClassEx( &wc ) ) return FALSE;

	return TRUE;
}

// ファイルドロップ(アイコン)
static BOOL _Function_DropProjectFile( HWND hWnd, char *lpszArgs )
{
	char path[MAX_PATH];
	long a;

	if( lpszArgs[0] == '"' )
	{
		for( a = 0; a < MAX_PATH-1; a++ )
		{
			if( lpszArgs[a+1] == '"' || lpszArgs[a+1] == '\0') break;
			path[a] = lpszArgs[a+1];
		}
	}
	else
	{
		for( a = 0; a < MAX_PATH-1; a++ )
		{
			if(                         lpszArgs[a+0] == '\0') break;
			path[a] = lpszArgs[a+0];
		}
	}
	path[a] = '\0';

	return TRUE;
}

//////////////////////////////////////////
// WinMain ///////////////////////////////
//////////////////////////////////////////

LRESULT CALLBACK WindowProc_Main( HWND hWnd, UINT msg, WPARAM w, LPARAM l );

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode )
{

	g_hInst = hInst;
/*
	AppExisting existing_window;
	static char *mutex_name   = "pxstage";
	static char *mapping_name = "map_pxstage";
	if( !existing_window.Check( mutex_name, mapping_name ) ) return 0;
*/

	// コモンコントロールのdllを初期化
	InitCommonControls();
	KeyControl_Clear();

	// 汎用のパスを取得-------------------------------------
	GetModuleFileName(      NULL, g_module_path, MAX_PATH );
	PathRemoveFileSpec(           g_module_path );
	sprintf(                      g_dir_profile, "%s\\profile_pxstage", g_module_path );
	sprintf(	                  g_dir_data   , "%s\\data"        , g_module_path );
	CreateDirectory(              g_dir_profile, NULL );
	WindowRect_Initialize    (    g_dir_profile );
//	FilePath_SetTemporaryPath(    g_dir_profile );
//	TriggerFile_SetTemporaryPath( g_dir_profile );

	strcpy( g_app_name, "pxStage"  );

	// ウインドウクラスを定義
	if( !_RegistWindowClass( hInst, _class_name, WindowProc_Main ) ) return FALSE;

    g_hMenu_Main   = LoadMenu( hInst, "MENU_MAIN" );

	//広さを指定
	g_MinimizeWidth  = VIEWDEFAULT_W + GetSystemMetrics(SM_CXFRAME)*2;
	g_MinimizeHeight = VIEWDEFAULT_H +
						GetSystemMetrics(SM_CYFRAME)*2 +
						GetSystemMetrics(SM_CYCAPTION) +
						GetSystemMetrics(SM_CYMENU) ;

	// ウインドウを生成
	g_hWnd_Main = CreateWindow(
		_class_name  ,
		g_app_name   ,
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		g_MinimizeWidth, g_MinimizeHeight,
		HWND_DESKTOP ,
		g_hMenu_Main ,
		hInst, NULL );

/*
	if( !existing_window.Mapping( mutex_name, mapping_name, g_hWnd_Main ) ) return 0;
*/
	// ウインドウ位置の読み込み
	if( !WindowRect_Load( g_hWnd_Main, g_main_rect_name, TRUE ) ) WindowRect_Centering( g_hWnd_Main );

	if( !DxDraw_Initialize( g_hWnd_Main, NULL, FALSE, FALSE ) ){ mbox_ERR( g_hWnd_Main, "dxdraw" ); return 0; }
	if( !DxDraw_MainSurface_Ready( g_hWnd_Main, GetSystemMetrics( SM_CXFULLSCREEN ), GetSystemMetrics( SM_CYFULLSCREEN ), _mag ) )
	{
		mbox_ERR( g_hWnd_Main, "main surface" ); return 0;
	}

	DxDraw_SetViewOffset( GetSystemMetrics( SM_CXFRAME   ) + 
						  GetSystemMetrics( SM_CXPADDEDBORDER ),
						  GetSystemMetrics( SM_CYFRAME   ) +
						  GetSystemMetrics( SM_CXPADDEDBORDER ) +
						  GetSystemMetrics( SM_CYCAPTION ) +
						  GetSystemMetrics( SM_CYMENU    ) );
	DxDraw_Text_Ready( NULL, 6, 12 );


	STAGESURFACES   surfs;

	surfs.mparts       = SfNo_MAPPARTS    ;
	surfs.attr1        = SfNo_ATTRIBUTE1  ;
	surfs.attr2        = SfNo_ATTRIBUTE2  ;
	surfs.unit_lcast   = SfNo_UNIT_lCast  ;
	surfs.unit_lsymbol = SfNo_UNIT_lSymbol;

	g_stage         = new dt_Stage( g_dir_data, MAX_MAP_W, MAX_MAP_L, &surfs );

	EditMode_Initialize(  g_hMenu_Main );
	if_GenericLoad(       g_hWnd_Main  );
	Interface_Initialize( g_hWnd_Main  );


	StageProject_Load2( g_hWnd_Main, "stage" );
	MainWindow_SetTitle( g_stage->Stage_GetName() );


	PostMessage( g_hWnd_Main, WM_PAINT, NULL, NULL );

//	g_hList_Room = CreateDialog( g_hInst, "DLG_LIST_ROOM", g_hWnd_Main, dlg_List_Room );

	MSG msg;
	//メッセージループを生成
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
//		if( !IsDialogMessage( g_hList_Room, &msg ) )
//		{
			TranslateMessage( &msg );//キーボード使用可能
			DispatchMessage(  &msg );//制御をWindowsに戻す
//		}
	}

	EditMode_Release       ();
	Interface_Release      ();
	DxDraw_Text_Release    ();
	DxDraw_Release( g_hWnd_Main );

	delete g_stage;

	return 1;
}

//////////////////////////////////////////
// グローバル関数 ////////////////////////
//////////////////////////////////////////
/*
long MainWindow_GetFPS( void )
{
	static               first     = 1;
	static unsigned long wait      = 0;
	static unsigned long count     = 0;
	static long          max_count = 0;

	unsigned long        a;

	if( first ){
		wait  = GetTickCount();
		first = 0;
	}

	a = GetTickCount();

	count++;
	if( wait +  1000 <= a ){
		wait += 1000;
		max_count = count;
		count     = 0;
	}
	return max_count;
}
*/
