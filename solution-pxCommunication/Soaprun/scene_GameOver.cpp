#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "utility_Draw/ListFrame.h"
#include "utility_Draw/PxImage.h"
#include "utility_Draw/FontWork.h"
#include "utility_Sound/SEFF.h"
#include "utility_Sound/BGM.h"

#include "scene_Buttons.h"
#include "Scene.h"
#include "Online.h"
#include "RequestResponse.h"

#define _CONTENTS_NUM 2

extern DWORD      g_color_bg_text ;
extern RECT       g_rc_wnd   ;
extern Scene      *g_scene   ;

static int        _count = 0;

void scene_GameOver_Initialize( void )
{
	_count = 0;
}

void scene_GameOver_Release( void )
{
}

void scene_GameOver_Reset( void )
{
	_count = 0;
	Request_Bye();
}

void scene_GameOver_Procedure( const BUTTONSSTRUCT* p_btns )
{

	DxDraw_PaintRect( &g_rc_wnd, g_color_bg_text  );
	FontWork_PutText( &g_rc_wnd,
		( g_rc_wnd.right  - g_rc_wnd.left ) / 2 - 32,
		( g_rc_wnd.bottom - g_rc_wnd.top - FONTWORK_FONTH) / 2,
		"ゲーム オーバー", 0 );

	if( _count++ > 60 * 2 )
	{
		FontWork_PutText( &g_rc_wnd,
			g_rc_wnd.left + 8,
			g_rc_wnd.bottom - 8 - FONTWORK_FONTH,
			"Ｏｋボタン で タイトルがめん。", 0 );

		if( p_btns->trg & KEY_BUTTON_1 )
		{
			g_scene->Reset_Set();
			return;
		}
	}



}
