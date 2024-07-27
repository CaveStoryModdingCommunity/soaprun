#include <StdAfx.h>

#include "../Dx/DxDraw.h"
#include "../Dx/DxSound.h"
#include "../Dx/DxInput.h"

#include "../Fixture/AppDirectory.h"
#include "../Fixture/AppExisting.h"
#include "../Fixture/WindowRect.h"
#include "../Fixture/DebugLog.h"
#include "../Fixture/ShiftJIS.h"
#include "../Fixture/KeyControl.h"
#include "../Fixture/MessageBox.h"
#include "../Fixture/URL.h"

#include "../ws/ws.h"
#include "../SoapCGI/CGI.h"

#include "resource.h"

#include "pxtone/pxtone.h"
#include "GameMain.h"
#include "Scene.h"
#include "utility_Sound/BGM.h"
#include "utility_Sound/SEFF.h"

#include "../DataTable/dt_Stage.h"

RECT         g_rc_wnd  = {           0, 0, WINDOW_W   , WINDOW_H    };
RECT         g_rc_fld  = {           0, 0, FIELDVIEW_W, FIELDVIEW_H };
RECT         g_rc_txt  = { FIELDVIEW_W, 0, WINDOW_W   , FIELDVIEW_H };
char         *g_app_name        = "Soaprun";
int          g_version          =         0;
HWND         g_hWnd             = NULL     ;


#define      _DEFAULT_MAG   1

static HMENU _hMenu             = NULL;
static char  *_rect_name        = "main.rect"       ;
static char  *_screen_mode_name = "main.screen_mode";
static char  *_class_name       = "Main"            ;
static bool  _b_window_active   = false;
static long  _mag               = _DEFAULT_MAG;

// 最小化
static void _InactiveWindow( void )
{
	if( _b_window_active )
	{
		_b_window_active = false;
		BGM_Inactive();
		SEFF_Noise_Inactive();
	}
}

// ウインドウサイズ
static void _ActiveWindow( void )
{
	if( !_b_window_active )
	{
		_b_window_active = true;
		SetFocus( g_hWnd );
		BGM_Active();
		SEFF_Noise_Active();
		KeyControl_Clear();
	}
}

// サウンドボリュームを開く
static BOOL _OpenVolumeControl( HWND hWnd )
{
	HINSTANCE hShell1;
	HINSTANCE hShell2;
	char path[ MAX_PATH];
	char path1[MAX_PATH];
	char path2[MAX_PATH];

	GetSystemDirectory( path, MAX_PATH );
	sprintf( path1, "%s\\Sndvol32.exe", path );
	strcpy(  path2, path );
	PathRemoveFileSpec( path2 );
	strcat(  path2, "\\Sndvol32.exe" );

	hShell1 = ShellExecute( hWnd,"open",path1,NULL,NULL,SW_SHOW);
	hShell2 = ShellExecute( hWnd,"open",path2,NULL,NULL,SW_SHOW);
	if( (int)hShell1 <= 32 && (int)hShell2 <= 32 ) return FALSE;

	return TRUE;
}

static BOOL _RegistWindowClass( HINSTANCE hInst, char *class_name, WNDPROC lpfnWndProc )
{
    WNDCLASSEX wc;

	memset( &wc, 0, sizeof(WNDCLASSEX) );

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpszClassName = class_name;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = lpfnWndProc;
    wc.hInstance     = hInst;        //インスタンス
    wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
//	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon         = LoadIcon( hInst, "0" );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW       );

    if( !RegisterClassEx( &wc ) ) return FALSE;

	return TRUE;
}

// Loadingを表示
static void _PutCenterLoading( void )
{
	RECT rcLoading = {0,0,128,16};

	// 文字を作る
	unsigned long color_back;
	unsigned long color_font;
	unsigned long color_font2;
	long r;
	RECT rect2;
	RECT rect[] = {
		{28,3,29,14},{29,3,33,7},{34,3,38,7},{38,3,39,14},{31,8,33,14},{34,8,36,14},{30,9,37,12},//開(7)
		{42,3,47,4},{48,3,53,4},{42,4,46,6},{49,4,53,6},{42,6,43,8},{52,6,53,8},{44,8,51,11},{42,11,47,14},{48,11,53,14},//発(9)
		{60,3,63,4},{56,4,67,5},{56,5,58,7},{65,5,67,7},{56,8,59,10},{59,6,64,9},{64,8,67,10},{60,9,63,13},{59,11,64,12},{56,13,67,14},//室(10)
		{70,3,79,10},{70,10,73,14},{80,7,83,9},{80,10,83,14},{84,7,87,10},{88,7,91,10},{86,9,89,12},{84,11,87,14},{88,11,91,14},//Pix(9)
		{92,7,98,11},{92,11,93,12},{92,12,98,14},{99,3,102,14},//el(4)
	};
	color_back  = DxDraw_GetPaintColor( RGB( 0x00, 0x00, 0x00 ) );
	color_font  = DxDraw_GetPaintColor( RGB( 0xF0, 0xF0, 0xF0 ) );
	color_font2 = DxDraw_GetPaintColor( RGB( 0x00, 0x00, 0xF0 ) );
	DxDraw_SubSurface_ReadyBlank( rcLoading.right - rcLoading.left, rcLoading.bottom - rcLoading.top, SfNo_LOADING, FALSE );

	DxDraw_PaintRect        ( &g_rc_wnd, color_back                    );
	DxDraw_PaintRect_Surface( &rcLoading  , color_back, SfNo_LOADING );
	for( r = 0; r < 39; r++ )
	{
		rect2 = rect[r];
		rect2.left  -=1;
		rect2.right +=1;
		DxDraw_PaintRect_Surface( &rect2, color_font2, SfNo_LOADING );
	}
	for( r = 0; r < 39; r++ )
	{
		DxDraw_PaintRect_Surface( &rect[r], color_font, SfNo_LOADING );
	}

	DxDraw_Put_Clip( &g_rc_wnd, g_rc_wnd.right/2-64, g_rc_wnd.bottom/2-8, &rcLoading, SfNo_LOADING ); 
	DxDraw_Flip_Fixed();// フリップ
}

static void _UpdateWindowSize( WPARAM wparam, BOOL bForce )
{
	long mag         = _mag        ;
//	BOOL bFullScreen = _bFullScreen;

	switch( LOWORD( wparam ) )
	{
	case IDM_MAGx1     : mag = 1; break;
	case IDM_MAGx2     : mag = 2; break;
//	case IDM_FULLSCREEN: bFullScreen = _bFullScreen ? FALSE : TRUE; break;
	}

	if( bForce || mag != _mag )
	{
		if( !DxDraw_ChangeMagnify( g_hWnd, mag ) ) return;
		GameMain_SizeRestore();
	}

	if( bForce /*|| bFullScreen != _bFullScreen*/ )
	{
//		if( bFullScreen ){ if( !DxDraw_CooperativeLevel( TRUE  ) ) return; }
//		else
		{ if( !DxDraw_CooperativeLevel( FALSE ) ) return; }
	}

	CheckMenuItem( _hMenu, IDM_MAGx1, MF_BYCOMMAND|MF_UNCHECKED );
	CheckMenuItem( _hMenu, IDM_MAGx2, MF_BYCOMMAND|MF_UNCHECKED );

	switch( mag )
	{
	case 1: CheckMenuItem( _hMenu, IDM_MAGx1, MF_BYCOMMAND|MF_CHECKED ); break;
	case 2: CheckMenuItem( _hMenu, IDM_MAGx2, MF_BYCOMMAND|MF_CHECKED ); break;
	}

//	if( bFullScreen ) CheckMenuItem( _hMenu, IDM_FULLSCREEN, MF_BYCOMMAND|MF_CHECKED   );
//	else              CheckMenuItem( _hMenu, IDM_FULLSCREEN, MF_BYCOMMAND|MF_UNCHECKED );

//	if( bFullScreen )
//	{
//		RECT rc;
//		DxDraw_SetViewOffset_Center();
//		GetWindowRect( _hWnd , &rc );
//		RedrawWindow(  _hWnd , &rc, NULL, RDW_INVALIDATE|RDW_ERASE );
//	}
//	else
//	{
		long w = WINDOW_W * mag;
		long h = WINDOW_H * mag;
		SetWindowPos( g_hWnd, NULL, 0, 0, w, h, SWP_NOMOVE );
//		if( _bFullScreen ) WindowRect_Centering( _hWnd );
//	}

	_mag = mag;
//	_bFullScreen = bFullScreen;
}

//メインウインドウプロシージャ
static LRESULT CALLBACK _WindowProc( HWND hWnd, UINT msg, WPARAM w, LPARAM l )
{

	switch( msg )
	{
	case WM_CREATE: _b_window_active = true; break;
	case WM_CLOSE : PostQuitMessage( 0 );    break;

	case WM_SIZE  :

		switch( w )
		{
		case SIZE_MINIMIZED: _InactiveWindow(); break;
		case SIZE_RESTORED:  _ActiveWindow  (); break;
		}
		break;

	case WM_KEYDOWN:
	case WM_KEYUP  : KeyControl_WM_MESSAGE( hWnd, msg, w ); break;

	// drag window.
	case WM_LBUTTONDOWN: PostMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, l ); break;

	// call menu.
	case WM_RBUTTONDOWN:

		POINT pt;
		pt.x = LOWORD( l ); pt.y = HIWORD( l ); ClientToScreen( hWnd, &pt );
		TrackPopupMenu( GetSubMenu( _hMenu, 0), TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
		break; 

	//コマンド
	case WM_COMMAND:

		switch( LOWORD( w ) )
		{
		case IDM_MAGx1     : 
		case IDM_MAGx2     : 
//		case IDM_FULLSCREEN:
			_UpdateWindowSize( w, FALSE ); break;

		case IDM_RESET     : GameMain_SetReset();                 break;
		case IDM_CLOSE     : SendMessage( hWnd, WM_CLOSE, 0, 0 ); break;
		case IDM_MINIMIZE  : ShowWindow ( hWnd, SW_MINIMIZE    ); break;
		case IDM_VOLUME    : _OpenVolumeControl( hWnd          ); break;
		default: return( DefWindowProc( hWnd, msg, w, l ) );
		}
		
		break;

	// 省電力モードの無効 / Alt のメニュー呼び出しを解除
	case WM_SYSCOMMAND:

		switch( w )
		{
		case SC_MONITORPOWER: break;
		case SC_KEYMENU:      break;// alt
		case SC_SCREENSAVE:   break;
		default: DefWindowProc( hWnd, msg, w, l ); break;
		}
		break;

	// IMEの解除
	case WM_IME_NOTIFY:

		if( w == IMN_SETOPENSTATUS )
		{
			HIMC hImc = ImmGetContext( hWnd );
			ImmSetOpenStatus( hImc, FALSE );
			ImmReleaseContext( hWnd, hImc );
		}
		break;

	default://上記スイッチ記述のないものはWindowsに処理を委ねる
		return DefWindowProc( hWnd, msg, w, l );
	}
    return 0;
}

//システムタスク
BOOL Main_SystemTask( void )
{
	MSG msg;

	//メッセージループを生成
	while( PeekMessage(   &msg, NULL, 0, 0, PM_NOREMOVE) )//|| !_b_window_active )
	{
		if( !GetMessage(  &msg, NULL, 0, 0 ) ) return FALSE; 
		TranslateMessage( &msg ); 
		DispatchMessage ( &msg );
	}

	return TRUE;
}

// システムメニューの変更
static void _SystemMenu( HWND hWnd )
{
	HMENU hMenu = GetSystemMenu( hWnd, FALSE );

	for( int m = GetMenuItemCount( hMenu ) - 1 ; m >= 0 ; m-- )
	{
		MENUITEMINFO info ;

		memset( &info, 0, sizeof(MENUITEMINFO) );
		info.cbSize = sizeof(MENUITEMINFO) ;
		info.fMask  = MIIM_ID | MIIM_TYPE ;

		if( GetMenuItemInfo( hMenu, (UINT)m, TRUE, &info ) )
		{
			switch( info.wID )
			{
			case SC_MAXIMIZE:
			case SC_SIZE    :
				DeleteMenu( hMenu, m, MF_BYPOSITION ) ;
				break;
			}
		}
	}
}

static void _ScreenMode_Save( void )
{
	FILE *fp = NULL;
	char path[ MAX_PATH ];
	sprintf( path, "%s\\%s", g_dir_profile, _screen_mode_name );
	fp = fopen( path, "wb" );
	if( fp )
	{
		fwrite( &_mag        , sizeof(long), 1, fp );
//		fwrite( &_bFullScreen, sizeof(BOOL), 1, fp );
		fclose( fp );
	}
}

static BOOL _ScreenMode_Load( void )
{
	BOOL  bReturn = FALSE;
	FILE* fp      = NULL ;
	char  path[ MAX_PATH ];

	sprintf( path, "%s\\%s", g_dir_profile, _screen_mode_name );
	fp = fopen( path, "rb" );
	if( fp )
	{
		if( fread( &_mag        , sizeof(long), 1, fp ) != 1 ) goto End;
//		if( fread( &_bFullScreen, sizeof(BOOL), 1, fp ) != 1 ) goto End;
		fclose( fp );
	}
	if( _mag != 1 && _mag != 2
//		&& _mag != 3 && _mag != 4 && _mag != 5
		) _mag = _DEFAULT_MAG;

	bReturn = TRUE;
End:
	if( !bReturn ) 
	{
		 _mag         = _DEFAULT_MAG;
//		 _bFullScreen = FALSE       ;
	}

	return bReturn;
}


void Main_SaveWindowRect( HWND hWnd )
{
	WindowRect_Save( hWnd, _rect_name );
	_ScreenMode_Save();
}


typedef struct{ char  a; char  b; unsigned char c; } STRUCT_1; // 3
typedef struct{ short a;                   } STRUCT_2; // 2
typedef struct{ char  a;                   } STRUCT_3; // 1
typedef struct{ char  a; short b;          } STRUCT_4; // 4
typedef struct{ short a; int   b;          } STRUCT_5; // 8
typedef struct{ int   a; short b;          } STRUCT_6; // 8
typedef struct{ char  a; int   b;          } STRUCT_7; // 8
typedef struct{ char  a; short b; char  c; } STRUCT_8; // 6
typedef struct{ char  a; char  c; short b; } STRUCT_9; // 4
typedef struct{ char  a; int   b; char  c; } STRUCT_a; //12
typedef struct{ char  a; unsigned char  c; int   b; } STRUCT_b; // 8
typedef struct{ char req_name[ 4 ]; bool  b_result; }STRUCT_c     ;


#include "../Fixture/RingBuffer.h"

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd )
{
	{
		int a;
		a = sizeof(STRUCT_1);
		a = sizeof(STRUCT_2);
		a = sizeof(STRUCT_3);
		a = sizeof(STRUCT_4);
		a = sizeof(STRUCT_5);
		a = sizeof(STRUCT_6);
		a = sizeof(STRUCT_7);
		a = sizeof(STRUCT_8);
		a = sizeof(STRUCT_9);
		a = sizeof(STRUCT_a);
		a = sizeof(STRUCT_b);
		a = sizeof(STRUCT_c);
		a = a;

		unsigned char us = 200;
		char          si = us;
		si = si;
	}

	g_hInst = hInst;

#ifdef NDEBUG

	AppExisting app_ext;
	static char *mutex_name   = "soaprun"    ;
	static char *mapping_name = "map_soaprun";

	if( app_ext.IsExisting( mutex_name, mapping_name ) )
	{
		MessageBox( NULL, "すでに起動しています。", g_app_name, MB_OK|MB_ICONEXCLAMATION );
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

	if( !( g_hEve_Exit = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) goto End;

	dlog( ".start." );

	CGI_Initialize();
	ws_Initialize ();

	KeyControl_Clear();
	if( !_RegistWindowClass( hInst, _class_name, _WindowProc ) ) goto End;
    _hMenu = LoadMenu( hInst, "MENU_MAIN" );

//	if( g_b_CGI ) RemoveMenu( GetSubMenu( _hMenu, 0 ), IDM_CLOSE, MF_BYCOMMAND );

	char title_text[ 100 ];

#ifdef NDEBUG
	sprintf( title_text, "%s",       g_app_name );
#else
	sprintf( title_text, "%s Debug", g_app_name );
#endif

	g_hWnd  = CreateWindow( _class_name, title_text ,
		WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, HWND_DESKTOP, NULL,hInst, NULL );

	_SystemMenu( g_hWnd );
	_ScreenMode_Load(   );

	if( !DxDraw_Initialize       ( g_hWnd, g_dir_data, FALSE, FALSE ) ){ mbox_ERR( g_hWnd, "dxdraw"       ); goto End; }
	if( !DxDraw_MainSurface_Ready( g_hWnd, WINDOW_W, WINDOW_H, _mag ) ){ mbox_ERR( g_hWnd, "main surface" ); goto End; }

	DxDraw_SetViewOffset( 0, 0        );
	DxDraw_Text_Ready   ( NULL, 6, 12 );
	_UpdateWindowSize   ( 0, TRUE     );

	if( !WindowRect_Load( g_hWnd, _rect_name, FALSE ) ) WindowRect_Centering( g_hWnd );

	{
		DWORD count = GetTickCount();

		_PutCenterLoading();
		DxInput_Initialize( hInst, g_hWnd );
		DxInput_SetNeutral();

		// pxtone
		BOOL b = pxtone_Ready( g_hWnd, 2, 44100, 16, 0.1f, TRUE, NULL );
//		BOOL b = pxtone_Ready( g_hWnd, 2, 11025,  8, 0.1f, TRUE, NULL );
		DxSound_SetDirectSoundPointer( pxtone_GetDirectSound() );

		// stage 
		STAGESURFACES stage_surf;
		memset( &stage_surf, 0, sizeof(STAGESURFACES) );

		if( !GameMain_Initialize() )
		{
			dlog( "ERR: Initialize Game." );
			goto End;
		}

		while( GetTickCount() < count + (1000 * 0.5f) ){}
	}

	GameMain_Loop( g_hWnd );

End:

	if( g_hEve_Exit ) CloseHandle( g_hEve_Exit );

	DxDraw_Text_Release(        ); dlog( "released: dx text" );
	DxDraw_Release     ( g_hWnd ); dlog( "released: dx draw" );
	DestroyWindow      ( g_hWnd ); dlog( "destroy window" );
	DestroyMenu        ( _hMenu ); dlog( "destroy menu" );

	CGI_Release(); dlog( "released: CGI" );
	ws_Release (); dlog( "released: ws"  );

	dlog( "End." );

	DebugLog_Release();
	AppDirectory_Release( &g_dir_profile );

	timeEndPeriod  ( 1 );

	return 1;
}
