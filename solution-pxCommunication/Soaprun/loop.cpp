#include <StdAfx.h>

#include "../Dx/DxDraw.h"     // Direct Draw
#include "../Fixture/KeyControl.h"

#include "utility_Draw/ListFrame.h"
#include "utility_Draw/FontWork.h"

#include "GameMain.h"
#include "loop.h"
#include "Scene.h"

#define _FLIPWAIT   17

static BOOL _bReset = FALSE;

extern RECT  g_rc_wnd;
extern Scene *g_scene;

BOOL Main_SystemTask( void );

static bool          _FPS_first     = true;
static unsigned long _FPS_wait      = 0;
static unsigned long _FPS_count     = 0;
static long          _FPS_max_count = 0;
/*
static long _GetFPS( void )
{

}
*/

unsigned long Loop_GetFPS()
{
	return _FPS_max_count;
}

void Loop_PutFPS( void )
{
	if( _FPS_first )
	{
		_FPS_wait  = GetTickCount();
		_FPS_first = false;
	}

	unsigned long a = GetTickCount();

	_FPS_count++;
	if( a >= _FPS_wait + 1000 )
	{
		_FPS_wait += 1000;
		_FPS_max_count = _FPS_count;
		_FPS_count     = 0;
	}
	if( _FPS_max_count > 999 ) _FPS_max_count = 999;
//	return _FPS_max_count;

	//	long fps  = _GetFPS();
	FontWork_PutNumber6( &g_rc_wnd, g_rc_wnd.right - 16, g_rc_wnd.bottom - 14  , _FPS_max_count, 2 );
}

static unsigned long _wait = 0;
static unsigned long _count;
static bool          _b_skip = false;

//void Loop_Reset()
//{
//	_wait  = _count = GetTickCount();
//}

BOOL Loop_Flip( void )
{
//	if( _count >= _wait + _FLIPWAIT ) _b_skip = true ;
//	else                              _b_skip = false;
	while( 1 )
	{
		if( !Main_SystemTask() ) return FALSE;
		Sleep( 1 );
		_count = GetTickCount();
		if( _count >= _wait + _FLIPWAIT ) break;
	}
	if( _count >= _wait + _FLIPWAIT * 5 )
		_wait  = _count;
	else
		_wait += _FLIPWAIT;

	DxDraw_Flip_Fixed();
	return TRUE;
}
/*
bool Loop_IsSkip()
{
	return _b_skip;
}
*/



BOOL Loop_IsEscapeExit( void )
{
	unsigned long bg_color;
	BOOL          bReturn = FALSE;

	if( !KeyControl_IsKey( VK_ESCAPE ) ) return FALSE;


//	return TRUE; // ‘¦I—¹B


	KeyControl_UpdateTrigger();
	bg_color = DxDraw_GetSurfaceColor( SfNo_COLOR_BGTEXT );

	ListFrame* lf_escape  = NULL;

	// title
	const SIMPLELIST lists_title[] =
	{
		{ 0, "CONTINUE..F1" , NULL, false },
		{ 0, "RESET.....F2" , NULL, false },
		{ 0, "EXIT......ESC", NULL, false },
	};
	lf_escape = new ListFrame( LISTFRAME_STATIC, 14, 3, POSFROM_LB, 12, 8, false );
	lf_escape->List_Set( lists_title, 3 );

	while( 1 )
	{
		// •â• PAUSE
		KeyControl_UpdateTrigger();
		if( KeyControl_IsKeyTrigger( VK_F1     ) ) break;
		if( KeyControl_IsKeyTrigger( VK_F2     ) ){ g_scene->Reset_Set(); break; }
		if( KeyControl_IsKeyTrigger( VK_ESCAPE ) ){ bReturn = TRUE      ; break; }

		DxDraw_PaintRect( &g_rc_wnd, bg_color );
		lf_escape->Put  ( &g_rc_wnd );
	
		if( !Loop_Flip() ){ bReturn = TRUE; break; }
	}

	if( lf_escape ) delete lf_escape;

	KeyControl_UpdateTrigger();
	return bReturn;
}


