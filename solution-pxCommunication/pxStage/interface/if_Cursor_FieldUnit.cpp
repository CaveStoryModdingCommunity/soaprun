#include <StdAfx.h>

#include "../../Fixture/KeyControl.h"

#include "../../DataTable/dt_Stage.h"

#include "Interface.h"
#include "if_Cursor.h"
#include "if_Table_FldObj.h"
#include "if_Table_Map.h"
#include "if_Tools.h"


extern HWND       g_hWnd_Main;
extern HINSTANCE  g_hInst;
extern cls_SCROLL g_ScrlMap_H;
extern cls_SCROLL g_ScrlMap_V;

extern dt_Stage*  g_stage;

BOOL CALLBACK dlg_FldObj( HWND hDlg, UINT msg, WPARAM w, LPARAM l );

static bool _Free( const POINT* p_cur )
{
	RECT*           p_field_rect;
	DT_FLDOBJ unit;
	int            x, y, u;

	p_field_rect = if_Table_Map_GetRect();

	if( p_cur->x <  p_field_rect->left   ||
		p_cur->x >= p_field_rect->right  ||
		p_cur->y <  p_field_rect->top    ||
		p_cur->y >= p_field_rect->bottom )
	{
		if_Table_FldObj_SetCursor( -1, -1 );
		if_Table_FldObj_VisibleSelect( FALSE );
		return false;
	}

	if_Table_FldObj_SetCursor( p_cur->x, p_cur->y );
	if_Table_FldObj_GetCursor( &x, &y );
	g_Cursor.p_field_rect = p_field_rect;

	// 左クリック
	if(       KeyControl_IsClickLeftTrigger()  )
	{
		switch( if_Tools_Mode_Get() )
		{
/*		case TOOLMODE_Erase:
			g_Cursor.focus  = enum_CursorFocus_FldObj;
			g_Cursor.action = enum_CursorAction_Drag_2;
			Table_Unit_Unit_Delete( x, y, x, y );
			break;
*/
		case TOOLMODE_Erase:
			if_Table_FldObj_VisibleSelect( TRUE );
			if_Table_FldObj_GetSelect( &g_Cursor.start_x, &g_Cursor.start_y, NULL, NULL );
			if_Table_FldObj_SetSelect( p_cur->x, p_cur->y, p_cur->x, p_cur->y );
			g_Cursor.focus  = enum_CursorFocus_FldObj;
			g_Cursor.action = enum_CursorAction_Drag_1;
			break;

		case TOOLMODE_Pen:
			g_Cursor.focus  = enum_CursorFocus_FldObj;

			u = g_stage->Unit_Search( x, y );

			if( u == -1 )
			{
				memset( &unit, 0, sizeof(DT_FLDOBJ) );
				unit.param1 = g_stage->Unit_copy_GetUnitNo();
				unit.x      = (short)x;
				unit.y      = (short)y;
				unit.flags  = UNITRECORD_FLAG_VALID;
				if( DialogBoxParam( g_hInst, "DLG_FIELDUNIT", g_hWnd_Main, dlg_FldObj, (LPARAM)(&unit) ) )
				{
					g_stage->Unit_Add( &unit );
				}
				KeyControl_Clear();
				g_Cursor.action = enum_CursorAction_Free;
			}
			else
			{
				g_Cursor.target = u;
				g_Cursor.action = enum_CursorAction_Drag_3;

			}
			break;
		}

	}
	else if( KeyControl_IsClickRightTrigger() )
	{
		switch( if_Tools_Mode_Get() )
		{
		case TOOLMODE_Pen:

			u = g_stage->Unit_Search( x, y );
			if( u != -1 )
			{
				g_stage->Unit_Get( u, &unit );
				if( DialogBoxParam( g_hInst, "DLG_FIELDUNIT", g_hWnd_Main, dlg_FldObj, (LPARAM)(&unit) ) )
				{
					g_stage->Unit_Set( u, &unit );
				}
				KeyControl_Clear();
				g_Cursor.action = enum_CursorAction_Free;
			}
			break;
/*
		case TOOLMODE_Erase:
			if_Table_FldObj_VisibleSelect( TRUE );
			if_Table_FldObj_GetSelect( &g_Cursor.start_x, &g_Cursor.start_y, NULL, NULL );
			if_Table_FldObj_SetSelect( p_cur->x, p_cur->y, p_cur->x, p_cur->y );
			g_Cursor.focus  = enum_CursorFocus_FldObj;
			g_Cursor.action = enum_CursorAction_Drag_1;
			break;
*/		}
		return true;
	}

	return true;
}

static bool _Drag1_Select_L( const POINT* p_cur )
{
	//ドラッグキャンセル
	if( KeyControl_IsClickRight() )
	{
		if_Table_FldObj_VisibleSelect( FALSE );
		g_Cursor.action = enum_CursorAction_Free;

	}
	// ドラッグ中
	else if( KeyControl_IsClickLeft() )
	{

		if_Table_FldObj_SetCursor( p_cur->x, p_cur->y );
		if_Table_FldObj_SetSelect( -1, -1, p_cur->x, p_cur->y );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->left, g_Cursor.p_field_rect->right , p_cur->x, &g_ScrlMap_H );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->top , g_Cursor.p_field_rect->bottom, p_cur->y, &g_ScrlMap_V );

	}
	// ドラッグ終了
	else
	{
		int x1, y1, x2, y2;
		if_Table_FldObj_GetSelect(   &x1, &y1, &x2, &y2 );

		switch( if_Tools_Mode_Get() )
		{
//		case TOOLMODE_Pen:
//			Table_Unit_Unit_Copy(   x1,  y1,  x2,  y2, FALSE );
//			break;
		case TOOLMODE_Erase:
			KeyControl_Clear();
			g_stage->Unit_Delete( x1,  y1,  x2,  y2 );
			break;
//		case TOOLMODE_Edit:
//			break;
		}

		if_Table_FldObj_VisibleSelect( FALSE );
		g_Cursor.action = enum_CursorAction_Free;
	}
	return true;
}

static bool _Drag2_Paste( const POINT* p_cur )
{
	//ドラッグキャンセル
	if( !KeyControl_IsClickLeft() || KeyControl_IsClickRight() )
	{
		g_Cursor.action = enum_CursorAction_Free;

	// ドラッグ中
	}
	else
	{
		if_Cursor_DragScroll( g_Cursor.p_field_rect->left, g_Cursor.p_field_rect->right , p_cur->x, &g_ScrlMap_H );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->top , g_Cursor.p_field_rect->bottom, p_cur->y, &g_ScrlMap_V );
	}
	return true;
}

static bool _Drag3_Move( const POINT* p_cur )
{
	DT_FLDOBJ unit;
	int x, y;

	g_stage->Unit_Get( g_Cursor.target, &unit );

	if_Table_FldObj_SetCursor( p_cur->x, p_cur->y );
	if_Table_FldObj_GetCursor(       &x,       &y );
	unit.x = (short)x;
	unit.y = (short)y;

	g_stage->Unit_Set( g_Cursor.target, &unit );

	//ドラッグキャンセル
	if( !KeyControl_IsClickLeft() || KeyControl_IsClickRight() )
	{
		g_Cursor.action = enum_CursorAction_Free;
	// ドラッグ中
	}
	else
	{
		if_Cursor_DragScroll( g_Cursor.p_field_rect->left, g_Cursor.p_field_rect->right , p_cur->x, &g_ScrlMap_H );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->top , g_Cursor.p_field_rect->bottom, p_cur->y, &g_ScrlMap_V );
	}
	return true;
}

// 音符編集カーソル
bool if_Cursor_Action_Table_Unit( const POINT* p_cur )
{
	bool b_ret = false;

	switch( g_Cursor.action )
	{
	case enum_CursorAction_Free  : b_ret = _Free(           p_cur ); break;
	case enum_CursorAction_Drag_1: b_ret = _Drag1_Select_L( p_cur ); break;
	case enum_CursorAction_Drag_2: b_ret = _Drag2_Paste(    p_cur ); break;
	case enum_CursorAction_Drag_3: b_ret = _Drag3_Move(     p_cur ); break;
	}

	return b_ret;
}

