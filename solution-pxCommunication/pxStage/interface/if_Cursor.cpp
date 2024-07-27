#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
#include "../../Fixture/KeyControl.h"

#include "../EditMode.h"

#include "Interface.h"
#include "SurfaceNo.h"
#include "if_Cursor.h"
#include "if_Table_Map.h"
#include "if_Table_Attribute.h"


// ドラッグスクロールウエイト
#define DRAGSCROLLWAIT 5

ACTIONCURSOR      g_Cursor;
extern cls_SCROLL g_ScrlMap_H;
extern cls_SCROLL g_ScrlMap_V;
extern cls_SCROLL g_ScrlAtt_H;
extern cls_SCROLL g_ScrlAtt_V;

bool if_Cursor_Action_Table_Map      ( const POINT* p_cur );
bool if_Cursor_Action_Table_Unit     ( const POINT* p_cur );
bool if_Cursor_Action_Table_Attribute( const POINT* p_cur );
bool if_Cursor_Action_Tools          ( const POINT* p_cur );
bool if_Cursor_Action_Palette        ( const POINT* p_cur );
//bool if_Cursor_Action_Table_Door     ( const POINT* p_cur );


///////////////////////////////////////
// 以下はグローバル ///////////////////
///////////////////////////////////////

void if_Cursor_Initialize( void )
{
	g_Cursor.target  = 0;
	g_Cursor.action  = enum_CursorAction_Free;
}

void if_Cursor_DragScroll( int frame1, int frame2, int cur, cls_SCROLL* scrl )
{
	if( cur <  frame1 || cur >= frame2 )
	{
		if( !g_Cursor.scroll_count )
		{
			if( cur <  frame1 ) scrl->Scroll( frame2 - frame1, -1 );
			if( cur >= frame2 ) scrl->Scroll( frame2 - frame1,  1 );
			g_Cursor.scroll_count = DRAGSCROLLWAIT;
		}
		else
		{
			g_Cursor.scroll_count--;
		}
	}else{
		g_Cursor.scroll_count = 0;
	}
}


bool if_Cursor_Action( const POINT* p_cur, bool bKeyboard )
{
	RECT *p_rect;

	// フリー ========================================
	if( g_Cursor.action == enum_CursorAction_Free )
	{
		g_Cursor.focus = enum_CursorFocus_None;
		while( 1 )
		{
			switch( EditMode_Get() )
			{

			// マップスクロール
			case enum_EditMode_Map      :
			case enum_EditMode_FldObj:
//			case enum_EditMode_FieldDoor:
				p_rect = if_Table_Map_GetRect();
				if( g_ScrlMap_H.Action( p_rect,  p_cur ) )
				{
					g_Cursor.focus  = enum_CursorFocus_MapH;
					g_Cursor.action = enum_CursorAction_Drag_1;
					break;
				}
				if( g_ScrlMap_V.Action(   p_rect,  p_cur ) ){
					g_Cursor.focus  = enum_CursorFocus_MapV;
					g_Cursor.action = enum_CursorAction_Drag_1;
					break;
				}
				switch( EditMode_Get() )
				{
				case enum_EditMode_Map:
					if( if_Cursor_Action_Tools     ( p_cur ) ) break;
					if( if_Cursor_Action_Palette   ( p_cur ) ) break;
					if( if_Cursor_Action_Table_Map ( p_cur ) ) break;
					break;
				case enum_EditMode_FldObj:
					if( if_Cursor_Action_Tools     ( p_cur ) ) break;
					if( if_Cursor_Action_Palette   ( p_cur ) ) break;
					if( if_Cursor_Action_Table_Unit( p_cur ) ) break;
					break;
/*				case enum_EditMode_FieldDoor:
					if( if_Cursor_Action_Tools     ( p_cur ) ) break;
					if( if_Cursor_Action_Palette   ( p_cur ) ) break;
					if( if_Cursor_Action_Table_Door( p_cur ) ) break;
					break;
*/				}
				break;

			// 属性スクロール
			case enum_EditMode_Attribute:

				p_rect = if_Table_Attribute_GetRect();
				if( g_ScrlAtt_H.Action(   p_rect,  p_cur ) )
				{
					g_Cursor.focus  = enum_CursorFocus_AttH;
					g_Cursor.action = enum_CursorAction_Drag_1;
					break;
				}
				if( g_ScrlAtt_V.Action(   p_rect,  p_cur ) )
				{
					g_Cursor.focus  = enum_CursorFocus_AttV;
					g_Cursor.action = enum_CursorAction_Drag_1;
					break;
				}
				if( if_Cursor_Action_Palette(         p_cur ) ) break;
				if( if_Cursor_Action_Table_Attribute( p_cur ) ) break;
				break;

			// RESIDENT イメージ
			case enum_EditMode_Resident:
				if( if_Cursor_Action_Palette(         p_cur ) ) break;
				break;
			}


			break;
		}

	}
	// ビジー ========================================
	else
	{
		bool bDrag = false;

		switch( g_Cursor.focus ){
		case enum_CursorFocus_MapH:
			p_rect = if_Table_Map_GetRect();
			if( g_ScrlMap_H.Action(   p_rect, p_cur ) ) bDrag = true;
			else g_Cursor.action  = enum_CursorAction_Free;
			break;
		case enum_CursorFocus_MapV:
			p_rect = if_Table_Map_GetRect();
			if( g_ScrlMap_V.Action(   p_rect, p_cur ) ) bDrag = true;
			else g_Cursor.action  = enum_CursorAction_Free;
			break;
		case enum_CursorFocus_AttH:
			p_rect = if_Table_Attribute_GetRect();
			if( g_ScrlAtt_H.Action(   p_rect, p_cur ) ) bDrag = true;
			else g_Cursor.action  = enum_CursorAction_Free;
			break;
		case enum_CursorFocus_AttV:
			p_rect = if_Table_Attribute_GetRect();
			if( g_ScrlAtt_V.Action(   p_rect, p_cur ) ) bDrag = true;
			else g_Cursor.action  = enum_CursorAction_Free;
			break;
		case enum_CursorFocus_Tools    : bDrag = if_Cursor_Action_Tools          ( p_cur ); break;
		case enum_CursorFocus_Palette  : bDrag = if_Cursor_Action_Palette        ( p_cur ); break;
		case enum_CursorFocus_Map      : bDrag = if_Cursor_Action_Table_Map      ( p_cur ); break;
		case enum_CursorFocus_FldObj     : bDrag = if_Cursor_Action_Table_Unit     ( p_cur ); break;
//		case enum_CursorFocus_FieldDoor     : bDrag = if_Cursor_Action_Table_Door     ( p_cur ); break;
		case enum_CursorFocus_Attribute: bDrag = if_Cursor_Action_Table_Attribute( p_cur ); break;
		}
		return true;
	}

	if( g_Cursor.focus == enum_CursorFocus_None ) return false;
	return true;
}
