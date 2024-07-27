#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "utility_Draw/ListFrame.h"
#include "utility_Draw/PxImage.h"
#include "utility_Draw/FontWork.h"
#include "utility_Sound/SEFF.h"
#include "utility_Sound/BGM.h"
#include "utility_Draw/SecondCounter.h"

#include "scene_Buttons.h"
#include "Scene.h"
#include "TalkBoard.h"
#include "Online.h"

#define _MAPNAME_X (FIELDVIEW_W + 4)
#define _MAPNAME_Y (4)

extern DWORD       g_color_bg_text;
extern RECT        g_rc_wnd      ;
extern Scene       *g_scene      ;
extern TalkBoard   *g_talkboard  ;
extern RingBuffer  *g_ring_msg   ;
extern RingBuffer  *g_ring_req   ;
extern RingBuffer  *g_ring_res   ;

static int           _count;
static SecondCounter _sec( 3 );

enum _CONNPHASE
{
	_CONNPHASE_Lobby ,
	_CONNPHASE_Server,
};

static _CONNPHASE _phase;

void scene_Connection_Initialize( void )
{
//	PxImage_Load( "title", SfNo_TITLE, FALSE  );
}

void scene_Connection_Release( void )
{
//	if( _lf_mode ) delete _lf_mode;
}

void scene_Connection_Reset( void )
{
	g_ring_msg ->Clear();
	g_talkboard->Clear();

//	g_ring_res ->Clear();
//	g_ring_req ->Clear();

	_count = 0;
	_phase = _CONNPHASE_Lobby;
	_sec.Reset();
}

void scene_Connection_Procedure( const BUTTONSSTRUCT* p_btns )
{
	if( _count++ == 10 ) Online_StartConnection();

	g_talkboard->Procedure();

	DxDraw_PaintRect( &g_rc_wnd, g_color_bg_text );

	FontWork_PutText( &g_rc_wnd, _MAPNAME_X, _MAPNAME_Y, "サーバー に せつぞく", 0 );

	g_talkboard->Put( &g_rc_wnd );
	_sec.Put( &g_rc_wnd, 8, 4 );

	if( Online_IsConnection() && !g_talkboard->IsBusy() )
	{
		g_scene->Set( SCENE_Game );
	}
}
