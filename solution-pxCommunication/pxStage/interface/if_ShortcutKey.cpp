#include <StdAfx.h>

#include "../../Fixture/KeyControl.h"

#include "../EditMode.h"

extern HMENU g_hMenu_Main;

BOOL if_ShortcutKey_Action( HWND hWnd )
{
	// ショートカット
//	if( KeyControl_IsKey( VK_CONTROL ) ){
//
//		if( KeyControl_IsKeyTrigger( 'Q' ) )
//		{
//			EditMode_Layer_Hide( EditMode_IsHideLayer() ? false : true );
//			return TRUE;
//		}

//		if( KeyControl_IsKeyTrigger( '1' ) ){ EditMode_Set( g_hMenu_Main, enum_EditMode_Map, 0 ); return TRUE; }
//		if( KeyControl_IsKeyTrigger( '2' ) ){ EditMode_Set( g_hMenu_Main, enum_EditMode_Map, 1 ); return TRUE; }
//		if( KeyControl_IsKeyTrigger( '3' ) ){ EditMode_Set( g_hMenu_Main, enum_EditMode_Map, 2 ); return TRUE; }
//		if( KeyControl_IsKeyTrigger( '4' ) ){ EditMode_Set( g_hMenu_Main, enum_EditMode_Map, 3 ); return TRUE; }
		
		
//	}

/*
	if( KeyControl_IsKey( VK_CONTROL ) )
	{
		if( if_ToolPanel_GetMode() != TOOLMODE_W )
		{
			if( KeyControl_GetWheel() > 0 ){
				if_ScalePanel_ShiftZoom( FALSE );
				return TRUE;
			}
			if( KeyControl_GetWheel() < 0 ){
				if_ScalePanel_ShiftZoom( TRUE  );
				return TRUE;
			}
		}
	}
*/
	return FALSE;
}