#include "StdAfx.h"

static const char* _p_dir_data = NULL;

void WindowRect_Initialize( const char* p_dir_data )
{
	_p_dir_data = p_dir_data;
}

// 親ウインドウの真中に配置
void WindowRect_Centering( HWND hWnd )
{
	HWND hParent;
	RECT rcParent, rcMe, rcDesk;
	long x, y;

	// デスクトップクライアント領域を取得
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesk, 0 );
	GetWindowRect( hWnd, &rcMe );

	// 親のRECTを取得
	if( hParent = GetParent( hWnd ) )
	{
		WINDOWPLACEMENT place;
		place.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement( hParent, &place );
		switch( place.showCmd )
		{
		case SW_SHOWMINIMIZED:
		case SW_HIDE         :
			SystemParametersInfo( SPI_GETWORKAREA, 0, &rcParent, 0 );
			break;
		default:
			GetWindowRect( hParent, &rcParent );
		}
	}
	else
	{
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rcParent, 0 );
	}

	// 位置
	x = rcParent.left + ( (rcParent.right  - rcParent.left) - (rcMe.right  - rcMe.left) ) /2;
	y = rcParent.top  + ( (rcParent.bottom - rcParent.top ) - (rcMe.bottom - rcMe.top ) ) /2;

	// スクリーン内に納まるよう移動
	if( x                          < rcDesk.left   ) x = rcDesk.left;
	if( y                          < rcDesk.top    ) y = rcDesk.top;
	if( x + (rcMe.right-rcMe.left) > rcDesk.right  ) x = rcDesk.right  - (rcMe.right-rcMe.left);
	if( y + (rcMe.bottom-rcMe.top) > rcDesk.bottom ) y = rcDesk.bottom - (rcMe.bottom-rcMe.top);
	
	SetWindowPos( hWnd, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_DRAWFRAME );

	PostMessage( hWnd, WM_SIZE, SIZE_RESTORED, 0 );
}

// ウインドウRECTのロード
// (WM_CREATEで呼ぶとフルスクリーン関数で失敗する)
BOOL WindowRect_Load( HWND hWnd, const char* name, BOOL bSize )
{
	FILE* fp;
	RECT  rect;
	char  path[MAX_PATH];

	RECT  rcDesk;
	long  max_w, max_h, min_w, min_h;
	long  showCmd = SW_NORMAL;

	if( !_p_dir_data ) return FALSE;

	sprintf( path, "%s\\%s", _p_dir_data, name );
	fp = fopen( path, "rb" );
	if( !fp ) return FALSE;

	fread( &rect,    sizeof(RECT), 1, fp );
	fread( &showCmd, sizeof(long), 1, fp );
	fclose( fp );

	// デスクトップクライアント領域を取得
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesk, 0 );
	// ウインドウ最大/最小サイズを取得
	max_w = GetSystemMetrics( SM_CXMAXIMIZED );
	max_h = GetSystemMetrics( SM_CYMAXIMIZED );
	min_w = GetSystemMetrics( SM_CXMIN       );
	min_h = GetSystemMetrics( SM_CYMIN       );

	// 小さすぎ
	if( rect.right  - rect.left < min_w ) rect.right  = rect.left + min_w;
	if( rect.bottom - rect.top  < min_h ) rect.bottom = rect.top  + min_h;
	// 大きすぎ
	if( rect.right  - rect.left > max_w ) rect.right  = rect.left + max_w;
	if( rect.bottom - rect.top  > max_h ) rect.bottom = rect.top  + max_w;

	// スクリーン内に納まる用に移動
	if( rect.left < rcDesk.left )
	{
		rect.right  += ( rcDesk.left - rect.left );
		rect.left   += ( rcDesk.left - rect.left );
	}
	if( rect.top  < rcDesk.top )
	{
		rect.bottom += ( rcDesk.top  - rect.top  );
		rect.top    += ( rcDesk.top  - rect.top  );
	}
	if( rect.right  > rcDesk.right )
	{
		rect.left   -= rect.right  - rcDesk.right;
		rect.right  -= rect.right  - rcDesk.right;
	}
	if( rect.bottom > rcDesk.bottom )
	{
		rect.top    -= rect.bottom - rcDesk.bottom;
		rect.bottom -= rect.bottom - rcDesk.bottom;
	}

	// 移動のみ／サイズも指定
	if( bSize ) MoveWindow(   hWnd, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, FALSE );
	else        SetWindowPos( hWnd, NULL, rect.left, rect.top, 0,0, SWP_NOSIZE );

	if( showCmd == SW_SHOWMAXIMIZED ) ShowWindow( hWnd, SW_SHOWMAXIMIZED );
	else                              ShowWindow( hWnd, SW_NORMAL        );

	return TRUE;
}
 
// ウインドウRECTのセーブ
BOOL WindowRect_Save( HWND hWnd, const char* name )
{
	FILE*           fp;
	WINDOWPLACEMENT place;
	char            path[MAX_PATH];
	RECT            rc;

	if( !_p_dir_data ) return FALSE;

	// ウインドウの状態を取得する
	if( !GetWindowPlacement( hWnd, &place ) ) return FALSE;
	// ノーマル表示の場合はスクリーン座標を取得
	if( place.showCmd == SW_NORMAL )
	{
		if( !GetWindowRect( hWnd, &rc ) ) return FALSE;
		place.rcNormalPosition = rc;
	}

	sprintf( path, "%s\\%s", _p_dir_data, name );
	fp = fopen( path, "wb" );
	if( !fp ) return FALSE;

	fwrite( &place.rcNormalPosition, sizeof(RECT), 1, fp );
	fwrite( &place.showCmd,          sizeof(long), 1, fp );
	fclose( fp );

	return TRUE;
}