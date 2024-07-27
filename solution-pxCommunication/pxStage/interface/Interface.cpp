#include <StdAfx.h>


#include "../../Dx/DxDraw.h"
#include "../../Fixture/KeyControl.h"

#include "../../DataTable/dt_MapData.h"

#include "../EditMode.h"

#include "SurfaceNo.h"
#include "Interface.h"
#include "if_Generic.h"
#include "if_Cursor.h"
#include "if_Table_Map.h"
#include "if_Table_FldObj.h"
//#include "if_Table_FieldDoor.h"
#include "if_Table_Attribute.h"
#include "if_Palette.h"
#include "if_ShortcutKey.h"
#include "if_Tools.h"

extern HWND g_hWnd_Main;

// 変数群 =======================================-

static BOOL _bInit = FALSE;
RECT        g_rcView;
cls_SCROLL  g_ScrlMap_H;
cls_SCROLL  g_ScrlMap_V;
cls_SCROLL  g_ScrlAtt_H;
cls_SCROLL  g_ScrlAtt_V;


// 関数群 =======================================-

// マウスカーソルの座標を取得
static void _GetMousePointer( POINT* p_pt )
{
	GetCursorPos(                p_pt );
	ScreenToClient( g_hWnd_Main, p_pt );
}

static long _CountFramePerSecond( void )
{
	static          BOOL bFirst    = TRUE;
	static unsigned long wait      = 0;
	static unsigned long count     = 0;
	static long          max_count = 0;

	if( bFirst ){
		wait   = GetTickCount();
		bFirst = FALSE;
	}

	count++;
	if( wait + 1000 <= GetTickCount() ){
		wait     += 1000;
		max_count = count;
		count     = 0;
	}

	return max_count;
}

//long g_test;

#define _FLIPWAIT     20



void Interface_Process( BOOL bDraw )
{
	POINT cur;

	if( _bInit )
	{

		KeyControl_UpdateTrigger();
		_GetMousePointer( &cur );

		// RECT ========================================================
		GetClientRect( g_hWnd_Main, &g_rcView );

		// Make Rect ====================================================
		if_Table_Map_SetRect(       &g_rcView );
		if_Table_Attribute_SetRect( &g_rcView );
		if_Tools_SetRect();
		if_Palette_SetRect();

		// Action ========================================================
		if( if_Cursor_Action( &cur, NULL ) ) bDraw = TRUE;
		if( if_ShortcutKey_Action( g_hWnd_Main )  ) bDraw = TRUE;

		// 表示 ==========================================================
		if( bDraw )
		{
			switch( EditMode_Get() )
			{
			case enum_EditMode_Map:
				if_Table_Map_Put_Table(        &g_rcView );
				if_Table_Map_Put_Cursor(       &g_rcView );
				if_Palette_Put( EditMode_Get() );
				if_Table_Map_Put_Scroll(       &g_rcView );
				if_Tools_Put();
				break;

			case enum_EditMode_FldObj:
				if_Table_Map_Put_Table(        &g_rcView );
				if_Table_FldObj_Put_Table(  &g_rcView );
				if_Table_FldObj_Put_Cursor( &g_rcView );
				if_Palette_Put( EditMode_Get() );
				if_Table_Map_Put_Scroll(       &g_rcView );
				if_Tools_Put();
				break;

/*			case enum_EditMode_FieldDoor:
				if_Table_Map_Put_Table(        &g_rcView );
				if_Table_FieldDoor_Put_Symbol( &g_rcView );
				if_Table_FieldDoor_Put_Cursor( &g_rcView );
				if_Palette_Put( EditMode_Get() );
				if_Table_Map_Put_Scroll(       &g_rcView );
				if_Tools_Put();
				break;
*/
			case enum_EditMode_Attribute:
				if_Table_Attribute_Put_Table(  &g_rcView );
				if_Table_Attribute_Put_Cursor( &g_rcView );
				if_Palette_Put(  EditMode_Get() );
				if_Table_Attribute_Put_Scroll( &g_rcView );
				break;

			case enum_EditMode_Resident:
				if_Table_Map_Put_Table(        &g_rcView );
				if_Table_Map_Put_Cursor(       &g_rcView );
				if_Palette_Put( EditMode_Get() );
				if_Table_Map_Put_Scroll(       &g_rcView );
				break;
			}
//			if_Table_Map_Put_CursorPosition( cur_x, cur_y );
			// フレーム数
			unsigned long fps;
			RECT          rcFps = {112, 96,136,104};
			fps = _CountFramePerSecond();
			DxDraw_Put(            g_rcView.right - 40 - 4,  2, &rcFps, SfNo_TENKEY );
			PutNumber6( &g_rcView, g_rcView.right - 16 - 4,  2, fps, 2 );

//			RECT rc = {0,48,128,64};
//			DxDraw_Put( 0, 0, &rc, SfNo_SYMBOL );

		}



		{
			static unsigned long wait = 0;
			static unsigned long count;

			while( 1 ){
				Sleep( 1 );
				count = GetTickCount();
				if( count >= wait + _FLIPWAIT ){
					if( count >= wait + _FLIPWAIT * 5 ) wait  = count;
					else                                wait += _FLIPWAIT;
					break;
				}
			}
			DxDraw_Flip_Fixed();
		}

	}

#ifdef _DEBUGXXXXX

#endif
}



BOOL Interface_Initialize( HWND hWnd )
{

	g_ScrlMap_H.Initialize( SfNo_SCROLL_H, FALSE ); 
	g_ScrlMap_V.Initialize( SfNo_SCROLL_V, TRUE  );
	g_ScrlAtt_H.Initialize( SfNo_SCROLL_H, FALSE ); 
	g_ScrlAtt_V.Initialize( SfNo_SCROLL_V, TRUE  );

	RECT rc = { 0, 0, FIELDGRID, FIELDGRID };
	unsigned long color;
	color = DxDraw_GetPaintColor( RGB( 0x10, 0x10, 0x40 ) );
	DxDraw_PaintRect_Surface( &rc, color, SfNo_MAPPARTS );
//	DxDraw_PaintRect_Surface( &rc, color, SfNo_UNITTABLE  );

	if_Cursor_Initialize();
	if_Table_Map_Initialize();
	if_Table_FldObj_Initialize();
	if_Table_Attribute_Initialize();
	if_Palette_Initialize(  hWnd );
	if_Tools_Initialize(     hWnd );
	_bInit = TRUE;

	return TRUE;
}

void Interface_Release()
{
	if_Palette_SavePosition();
	if_Tools_SavePosition();
}



