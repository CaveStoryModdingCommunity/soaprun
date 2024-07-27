#include <StdAfx.h>

#include "../../Fixture/KeyControl.h"

#include "../EditMode.h"

#include "Interface.h"
#include "cls_Scroll.h"
#include "if_Cursor.h"
#include "if_Tools.h"


extern HWND g_hWnd_Main;

static bool _Free( const POINT* p_cur )
{
	RECT *p_field_rect;

	p_field_rect = if_Tools_GetRect();

	if( p_cur->x <  p_field_rect->left   ||
		p_cur->x >= p_field_rect->right  ||
		p_cur->y <  p_field_rect->top    ||
		p_cur->y >= p_field_rect->bottom )
	{
		return false;
	}

	g_Cursor.p_field_rect = p_field_rect;
	g_Cursor.drag_xpos    = p_cur->x;
	g_Cursor.drag_ypos    = p_cur->y;
	
	// クリック
	if(       KeyControl_IsClickLeftTrigger() || KeyControl_IsClickRightTrigger() )
	{
		g_Cursor.focus  = enum_CursorFocus_Tools;

		int ret;
		ret = if_Tools_GetHitButton_Visible( p_cur->x, p_cur->y );
		if( ret != -1 ){ if_Tools_Visible_Reverse( ret ); return true; }
		ret = if_Tools_GetHitButton_EditLayer( p_cur->x, p_cur->y );
		if( ret != -1 ){ if_Tools_EditLayer_Set(   ret ); return true; }

//		if( if_Tools_DoorVisible_GetHitButton( p_cur->x, p_cur->y ) ){ if_Tools_DoorVisible_Reverse(); return true; }
		if( if_Tools_AttrVisible_GetHitButton( p_cur->x, p_cur->y ) ){ if_Tools_AttrVisible_Reverse(); return true; }

		switch( if_Tools_GetHitButton_Mode( p_cur->x, p_cur->y ) )
		{
		case TOOLMODE_None:
			if_Tools_GetPosition( &g_Cursor.start_x, &g_Cursor.start_y );
			g_Cursor.action = enum_CursorAction_Drag_1;
			return true;
		case TOOLMODE_Pen:   // 追加
			g_Cursor.action = enum_CursorAction_Drag_2;
			if_Tools_Mode_Set( TOOLMODE_Pen );
			return true;
		case TOOLMODE_Erase: // 削除
			g_Cursor.action = enum_CursorAction_Drag_2;
			if_Tools_Mode_Set( TOOLMODE_Erase );
			return true;
		case TOOLMODE_Edit:  // 編集
			g_Cursor.action = enum_CursorAction_Drag_2;
			if_Tools_Mode_Set( TOOLMODE_Edit );
			return true;
		}

	}else{
		return true;
	}

	return true;
}

static bool _Drag1_Move( const POINT* p_cur )
{
	RECT *p_field_rect;
	bool bDrag = false;

	p_field_rect          = if_Tools_GetRect();
	g_Cursor.focus        = enum_CursorFocus_Tools;
	g_Cursor.p_field_rect = p_field_rect;

	//ドラッグキャンセル
	if( KeyControl_IsClickLeft() && KeyControl_IsClickRight() ){

		if_Tools_SetPosition( g_Cursor.start_x, g_Cursor.start_y );
		g_Cursor.action = enum_CursorAction_Free;

		return true;
	}

	if( KeyControl_IsClickLeft() ){
		if_Tools_SetPosition(
			g_Cursor.start_x + p_cur->x - g_Cursor.drag_xpos,
			g_Cursor.start_y + p_cur->y - g_Cursor.drag_ypos );
	}else{
		g_Cursor.action = enum_CursorAction_Free;
	}

	return true;
}

static bool _Drag2_Mode( const POINT* p_cur )
{
	//ドラッグキャンセル
	if( !KeyControl_IsClickLeft() || KeyControl_IsClickRight() ){
		g_Cursor.action = enum_CursorAction_Free;
	}
	return true;

}


// 音符編集カーソル
bool if_Cursor_Action_Tools( const POINT* p_cur )
{
	bool b_ret = false;

	switch( g_Cursor.action ){
	case enum_CursorAction_Free:   b_ret = _Free(       p_cur ); break;
	case enum_CursorAction_Drag_1: b_ret = _Drag1_Move( p_cur ); break;
	case enum_CursorAction_Drag_2: b_ret = _Drag2_Mode( p_cur ); break;
	}

	return b_ret;
}
