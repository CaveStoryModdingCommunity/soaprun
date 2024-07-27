#include <StdAfx.h>

#include "../../Fixture/KeyControl.h"

#include "../../DataTable/dt_Stage.h"

#include "Interface.h"
#include "if_Cursor.h"
#include "if_Table_Map.h"
#include "if_Tools.h"

extern dt_Stage* g_stage;

extern cls_SCROLL g_ScrlMap_H;
extern cls_SCROLL g_ScrlMap_V;

static bool _Free( const POINT* p_cur )
{
	RECT *p_field_rect;

	p_field_rect = if_Table_Map_GetRect();

	if( p_cur->x <  p_field_rect->left   ||
		p_cur->x >= p_field_rect->right  ||
		p_cur->y <  p_field_rect->top    ||
		p_cur->y >= p_field_rect->bottom )
	{
		if_Table_Map_SetCursor( -1, -1 );
		if_Table_Map_VisibleSelect( FALSE );
		return false;
	}

	if_Table_Map_SetCursor( p_cur->x, p_cur->y );
	g_Cursor.p_field_rect = p_field_rect;
	

	// 左クリック
	if(       KeyControl_IsClickLeftTrigger()  )
	{
		g_Cursor.focus  = enum_CursorFocus_Map;
		g_Cursor.action = enum_CursorAction_Drag_2;
		int x, y;
		if_Table_Map_GetCursor( &x, &y );

		int layer = if_Tools_EditLayer_Get();
		for( int l = 0; l < MAPLAYERNUM; l++ )
		{
			if( layer == MAPLAYERNUM || l == layer )g_stage->Map_copy_Paste( l, x, y );
		}
	}
	else if( KeyControl_IsClickRightTrigger() )
	{
		if_Table_Map_VisibleSelect( TRUE );
		if_Table_Map_GetSelect( &g_Cursor.start_x, &g_Cursor.start_y, NULL, NULL );
		if_Table_Map_SetSelect( p_cur->x, p_cur->y, p_cur->x, p_cur->y );
		g_Cursor.focus  = enum_CursorFocus_Map;
		g_Cursor.action = enum_CursorAction_Drag_1;
	}
	else
	{
		return true;
	}

	return true;
}

static bool _Drag1_Select( const POINT* p_cur )
{
	//ドラッグキャンセル
	if( KeyControl_IsClickLeft() ){

		if_Table_Map_VisibleSelect( FALSE );
		g_Cursor.action = enum_CursorAction_Free;

	// ドラッグ中
	}else if( KeyControl_IsClickRight() )
	{
		if_Table_Map_SetCursor( p_cur->x, p_cur->y );
		if_Table_Map_SetSelect( -1, -1, p_cur->x, p_cur->y );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->left, g_Cursor.p_field_rect->right , p_cur->x, &g_ScrlMap_H );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->top , g_Cursor.p_field_rect->bottom, p_cur->y, &g_ScrlMap_V );

	// ドラッグ終了
	}
	else
	{
		int x1, y1, x2, y2;
		if_Table_Map_GetSelect( &x1, &y1, &x2, &y2 );

		int layer = if_Tools_EditLayer_Get();
		for( int l = 0; l < MAPLAYERNUM; l++ )
		{
			if( layer == MAPLAYERNUM || l == layer ) g_stage->Map_copy_Copy( l, x1,  y1,  x2,  y2, FALSE );
		}

		


		if_Table_Map_VisibleSelect( FALSE );
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
		int x, y;
		if_Table_Map_SetCursor( p_cur->x, p_cur->y );
		if_Table_Map_GetCursor( &x, &y );

		int layer = if_Tools_EditLayer_Get();
		for( int l = 0; l < MAPLAYERNUM; l++ )
		{
			if( layer == MAPLAYERNUM || l == layer ) g_stage->Map_copy_Paste( l, x, y );
		}

//		Table_Map_Paste( EditMode_Layer_Get(), x, y );


		if_Cursor_DragScroll( g_Cursor.p_field_rect->left, g_Cursor.p_field_rect->right , p_cur->x, &g_ScrlMap_H );
		if_Cursor_DragScroll( g_Cursor.p_field_rect->top , g_Cursor.p_field_rect->bottom, p_cur->y, &g_ScrlMap_V );
	}
	return true;
}


// 音符編集カーソル
bool if_Cursor_Action_Table_Map( const POINT* p_cur )
{
	bool b_ret = false;

	switch( g_Cursor.action ){
	case enum_CursorAction_Free:   b_ret = _Free(         p_cur ); break;
	case enum_CursorAction_Drag_1: b_ret = _Drag1_Select( p_cur ); break;
	case enum_CursorAction_Drag_2: b_ret = _Drag2_Paste(  p_cur ); break;
	}

	return b_ret;
}

