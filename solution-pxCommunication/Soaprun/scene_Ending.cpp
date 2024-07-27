#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "utility_Draw/ListFrame.h"
#include "utility_Draw/PxImage.h"
#include "utility_Draw/FontWork.h"
#include "utility_Draw/Pane.h"
#include "utility_Sound/SEFF.h"
#include "utility_Sound/BGM.h"

#include "scene_Buttons.h"
#include "Scene.h"
#include "Online.h"
#include "RequestResponse.h"
#include "GameRecord.h"

#define _CONTENTS_NUM 2

extern DWORD      g_color_bg_text;
extern RECT       g_rc_wnd       ;
extern Scene      *g_scene       ;
extern GameRecord *g_grec        ;

static Pane       *_snap = NULL;

void scene_Ending_Initialize( void )
{
	_snap = new Pane();
	_snap->SetType_Image( 0, 0, 96, 128, SfNo_EndSnaps );

}

void scene_Ending_Release( void )
{
	if( _snap ) delete _snap; _snap = NULL;
}

void scene_Ending_Reset( void )
{
	Request_Bye();
	BGM_Play( BGM_Ending );
}


void scene_Ending_Procedure( const BUTTONSSTRUCT* p_btns )
{
	DxDraw_PaintRect( &g_rc_wnd, g_color_bg_text  );

//	int x = ( g_rc_wnd.right  - g_rc_wnd.left ) / 2 - 32;
//	int y = ( g_rc_wnd.bottom - g_rc_wnd.top - FONTWORK_FONTH) / 2;
	int x = ( g_rc_wnd.right  - g_rc_wnd.left ) / 2 + 32;
	int y = ( g_rc_wnd.bottom - g_rc_wnd.top - FONTWORK_FONTH) / 2 - 32;

	FontWork_PutText( &g_rc_wnd, x, y, "ゴール", 0 ); y += 24; x += 16;

	char param2 = g_grec->Get();
	char point  = 0;

	if( !param2                   ){ FontWork_PutText( &g_rc_wnd, x, y, "・スッピンピン"    , 0 ); y += 12; }
	if( param2 & EQUIPFLAG_SWORD  ){ FontWork_PutText( &g_rc_wnd, x, y, "・ぼうれいのけん"  , 0 ); y += 12; point++; }
	if( param2 & EQUIPFLAG_CROWN  ){ FontWork_PutText( &g_rc_wnd, x, y, "・はしゃのかんむり", 0 ); y += 12; point++; }
	if( param2 & EQUIPFLAG_SHIELD ){ FontWork_PutText( &g_rc_wnd, x, y, "・スコールシールド", 0 ); y += 12; point++; }

	_snap->SetAnimation( point, 0, 0, 0 );
	_snap->Put( &g_rc_wnd, 96, (WINDOW_H - _snap->H() )/2 );
}
