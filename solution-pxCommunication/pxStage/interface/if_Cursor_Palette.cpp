#include <StdAfx.h>

#include "../../Fixture/KeyControl.h"
#include "../../Fixture/MessageBox.h"
#include "../../DataTable/dt_Stage.h"

#include "../EditMode.h"

#include "Interface.h"
#include "if_Cursor.h"
#include "if_Palette.h"
#include "if_Table_Map.h"
#include "if_Tools.h"


extern HWND      g_hWnd_Main;
extern dt_Stage* g_stage    ;

static bool _Free( const POINT* p_cur )
{
	RECT *p_field_rect;

	p_field_rect = if_Palette_GetRect();

	if( p_cur->x <  p_field_rect->left   ||
		p_cur->x >= p_field_rect->right  ||
		p_cur->y <  p_field_rect->top    ||
		p_cur->y >= p_field_rect->bottom )
	{
		if_Palette_SetCursor( -1, -1 );
		if_Palette_VisibleSelect( false );
		return false;
	}

	if_Palette_SetCursor( p_cur->x, p_cur->y );
	g_Cursor.p_field_rect = p_field_rect;
	g_Cursor.drag_xpos    = p_cur->x;
	g_Cursor.drag_ypos    = p_cur->y;
	
	// クリック
	if(       KeyControl_IsClickLeftTrigger() || KeyControl_IsClickRightTrigger() ){
		if_Palette_GetSelect( &g_Cursor.start_x, &g_Cursor.start_y, NULL, NULL );
		g_Cursor.focus  = enum_CursorFocus_Palette;

		// select
		if(       if_Palette_IsMapParts(  p_cur->x, p_cur->y ) )
		{
			if_Palette_VisibleSelect( true );
			if_Palette_SetSelect( p_cur->x, p_cur->y, p_cur->x, p_cur->y );
			g_Cursor.action = enum_CursorAction_Drag_1;
			if_Tools_Mode_Set( TOOLMODE_Pen );
		}
		// close
		else if( if_Palette_IsCloseButton( p_cur->x, p_cur->y ) )
		{
			if_Palette_GetPosition( &g_Cursor.start_x, &g_Cursor.start_y );
			if_Palette_SetPosition(  g_Cursor.start_x, 32767 );
			g_Cursor.action = enum_CursorAction_Drag_2;
		}
		else if( if_Palette_IsLoadButton( p_cur->x, p_cur->y ) )
		{
			if( g_stage->Stage_ReloadImages() ) mbox_ERR  ( g_hWnd_Main,        "Reload Images"       );
			else                                MessageBox( g_hWnd_Main, "OK.", "Reload Image", MB_OK );
			g_Cursor.action = enum_CursorAction_Free;
			KeyControl_Clear();
		}

/*		// load
		}else if( if_Palette_IsLoadButton( p_cur->x, p_cur->y ) ){
			switch( EditMode_Get() ){
//			case enum_EditMode_Attribute:
//			case enum_EditMode_Map      : Palette_SelectAndLoad_Parts(    g_hWnd_Main ); break;
			case enum_EditMode_Resident : Palette_SelectAndLoad_Resident( g_hWnd_Main ); break;
			default: return true;
			}
			g_Cursor.action = enum_CursorAction_Free;
			KeyControl_Clear();
		// move
		}
*/		else
		{
			if_Palette_GetPosition( &g_Cursor.start_x, &g_Cursor.start_y );
			g_Cursor.action = enum_CursorAction_Drag_3;
		}
	}else{
		return true;
	}

	return true;
}

static bool _Drag1_Select( const POINT* p_cur )
{
	int layer = 0;
	
	switch( EditMode_Get() )
	{
	case enum_EditMode_Map      : layer = if_Tools_EditLayer_Get(); if( layer >= MAPLAYERNUM       ) layer = 0; break;
	case enum_EditMode_Attribute: layer = EditMode_Layer_Get(    ); if( layer >= ATTRIBUTELAYERNUM ) layer = 0; break;
	}

	//ドラッグ終了
	if( !KeyControl_IsClickLeft() && !KeyControl_IsClickRight() )
	{
		int x1, y1, x2, y2;
		if_Palette_GetSelect( &x1, &y1, &x2, &y2 );
		switch( EditMode_Get() )
		{
		case enum_EditMode_Map      : g_stage->Map_copy_Copy          ( layer, x1,  y1,  x2,  y2, true    ); break;
		case enum_EditMode_FldObj: g_stage->Unit_copy_Copy         (        x1,  y1,  x2,  y2, true    ); break;
///		case enum_EditMode_FieldDoor: if_Palette_SetSelectedPartsIndex( ( y1 / 2 ) * ATTRIBUTE_NUM_H + x1 ); break;
		case enum_EditMode_Attribute: if_Palette_SetSelectedPartsIndex(   y1       * ATTRIBUTE_NUM_H + x1 ); break;
		}
		if_Palette_VisibleSelect( false );
		g_Cursor.action = enum_CursorAction_Free;
	}

	if( EditMode_Get() != enum_EditMode_FldObj/* &&
		EditMode_Get() != enum_EditMode_FieldDoor */) if_Palette_SetSelect( -1, -1, p_cur->x, p_cur->y );
	return true;
}

static bool _Drag2_Close( const POINT* p_cur )
{
	//ドラッグキャンセル
	if( !KeyControl_IsClickLeft() || KeyControl_IsClickRight() )
	{
		g_Cursor.action = enum_CursorAction_Free;
	}
	return true;

}

static bool _Drag3_Move( const POINT* p_cur )
{
	RECT *p_field_rect;
	bool bDrag = false;

	p_field_rect          = if_Palette_GetRect();
	g_Cursor.focus        = enum_CursorFocus_Palette;
	g_Cursor.p_field_rect = p_field_rect;

	//ドラッグキャンセル
	if( KeyControl_IsClickLeft() && KeyControl_IsClickRight() )
	{
		if_Palette_SetPosition( g_Cursor.start_x, g_Cursor.start_y );
		g_Cursor.action = enum_CursorAction_Free;

		return true;
	}

	if( KeyControl_IsClickLeft() )
	{
		if_Palette_SetPosition(
			g_Cursor.start_x + p_cur->x - g_Cursor.drag_xpos,
			g_Cursor.start_y + p_cur->y - g_Cursor.drag_ypos );
	}else{
		g_Cursor.action = enum_CursorAction_Free;
	}

	return true;
}

// 音符編集カーソル
bool if_Cursor_Action_Palette( const POINT* p_cur )
{
	bool b_ret = false;

	switch( g_Cursor.action )
	{
	case enum_CursorAction_Free:   b_ret = _Free(         p_cur ); break;
	case enum_CursorAction_Drag_1: b_ret = _Drag1_Select( p_cur ); break;
	case enum_CursorAction_Drag_2: b_ret = _Drag2_Close(  p_cur ); break;
	case enum_CursorAction_Drag_3: b_ret = _Drag3_Move(   p_cur ); break;
	}

	return b_ret;
}
