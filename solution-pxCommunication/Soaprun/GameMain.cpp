#include <StdAfx.h>

#include "../Dx/DxDraw.h"    
#include "../Fixture/DebugLog.h"
#include "../Fixture/Random.h"

#include "utility_Input/Buttons.h"
#include "utility_Draw/PxImage.h"
#include "utility_Draw/Surface.h"
#include "utility_Draw/ScreenCurtain.h"
#include "utility_Sound/SEFF.h"

#include "SharedDefines.h"
#include "loop.h"
#include "Scene.h"
#include "scene_Title.h"
#include "scene_Connection.h"
#include "scene_Buttons.h"
#include "scene_Game.h"
#include "scene_GameOver.h"
#include "scene_Ending.h"
#include "TalkBoard.h"
#include "GameRecord.h"

#include "Online.h"

#include "Triangle.h"

extern RECT g_rc_wnd       ;

DWORD       g_color_bg_field;
DWORD       g_color_bg_text ;
DWORD       g_color_text   ;
Scene       *g_scene     = NULL;
RingBuffer  *g_ring_msg  = NULL;
RingBuffer  *g_ring_req  = NULL;
RingBuffer  *g_ring_res  = NULL;
TalkBoard   *g_talkboard = NULL;

Triangle    *g_tri       = NULL;

Random      g_ran( 0x12, 0xf2 );

GameRecord  *g_grec      = NULL;

static SCENE _old_scene;

void Main_SaveWindowRect( HWND hWnd );

//////////////////
// global
//////////////////

void GameMain_SetReset()
{
	g_scene->Reset_Set();
}

void GameMain_SizeRestore( void )
{
	long restore;
}

bool GameMain_Initialize( void )
{
	bool b_ret = false;

	if( !Surface_Initialize() ) goto End;
	if( !SEFF_Initialize   () ) goto End;

	g_scene     = new Scene     ( SCENE_Title         );
	g_ring_msg  = new RingBuffer( BUFFERSIZE_RING_MSG );
	g_ring_req  = new RingBuffer( BUFFERSIZE_RING_REQ );
	g_ring_res  = new RingBuffer( BUFFERSIZE_RING_RES );
	g_talkboard = new TalkBoard ( g_ring_msg          );

	g_tri       = new Triangle  (                     );
	g_grec      = new GameRecord();

	//	g_fade  = new ScreenCurtain();

	_old_scene = g_scene->Get();

	// ‰Šú‰»
	scene_Title_Initialize  ();
	scene_Buttons_Initialize();
	if( !scene_Game_Initialize() ) goto End;
	scene_Ending_Initialize();


	g_color_bg_field = DxDraw_GetSurfaceColor( SfNo_COLOR_BGFIELD );
	g_color_bg_text  = DxDraw_GetSurfaceColor( SfNo_COLOR_BGTEXT  );
	g_color_text     = DxDraw_GetSurfaceColor( SfNo_COLOR_TEXT    );

	if( !Online_Initialize() ) goto End;

	b_ret = true;
End:
	return b_ret;
}

void GameMain_Loop( HWND hWnd )
{
	BUTTONSSTRUCT btns     ;
	SCENE         now_scene;

	Buttons_ReloadConfig( );
	Buttons_Reset ( &btns );
//	Loop_Reset();

	while( g_scene->Get() != SCENE_Exit )
	{
		// escape
		if( g_scene->Get() != SCENE_Buttons && Loop_IsEscapeExit() ) break;

		// reset
		if( g_scene->Reset_Is() )
		{
			Online_Shutdown ();
			g_scene->Reset_Remove();
			Buttons_ReloadConfig( );
			Buttons_Reset ( &btns );
			scene_Title_Reset    ();
			g_scene->Set( SCENE_Title );
		}

		// “ü—Í
		btns.btn = Buttons_GetButtons();
		Buttons_UpdateTriggers( &btns );

		now_scene = g_scene->Get();
		if( _old_scene != now_scene )
		{
			switch( now_scene )
			{
			case SCENE_Game      : scene_Game_Reset      (); break;
			case SCENE_GameOver  : scene_GameOver_Reset  (); break;
			case SCENE_Ending    : scene_Ending_Reset    (); break;
			case SCENE_Connection: scene_Connection_Reset(); break;
			}
			_old_scene = now_scene;
		}

		switch( now_scene )
		{
		case SCENE_Title     : scene_Title_Procedure     ( &btns ); break;
		case SCENE_Connection: scene_Connection_Procedure( &btns ); break;
		case SCENE_Buttons   : scene_Buttons_Procedure   (       ); break;
		case SCENE_Game      : scene_Game_Procedure      ( &btns ); break;
		case SCENE_Ending    : scene_Ending_Procedure    ( &btns ); break;
		case SCENE_GameOver  : scene_GameOver_Procedure  ( &btns ); break;
		}

		Loop_PutFPS();
		if( !Loop_Flip() ) g_scene->Set( SCENE_Exit );
	}

	SetEvent( g_hEve_Exit );

	if( !Online_WaitExit( 100 ) ) dlog( "online thread timeout." );

	if( g_tri ) delete g_tri; g_tri = NULL;

	delete g_scene         ; dlog( "delete: g scene"         );
	delete g_ring_msg      ; dlog( "delete: g ring"          );
	delete g_talkboard     ; dlog( "delete: g talkboard"     );
	delete g_grec          ;

	scene_Title_Release  (); dlog( "released: scene title"   );
	scene_Buttons_Release(); dlog( "released: scene buttons" );
	scene_Game_Release   (); dlog( "released: scene game"    );
	scene_Ending_Release (); dlog( "released: scene Ending"  );

	Main_SaveWindowRect( hWnd );

	SEFF_Release();
}
