#include <StdAfx.h>

#include "../Dx/DxInput.h"
#include "../Dx/DxDraw.h"
#include "../Fixture/KeyControl.h"


#include "utility_Draw/FontWork.h"
#include "utility_Draw/ListFrame.h"
#include "utility_Sound/SEFF.h"


#include "Scene.h"


extern RECT          g_rc_wnd;
extern DWORD         g_color_bg_text ;

extern Scene         *g_scene   ;

static unsigned char _ani_wait  ;
static unsigned char _pos       ;
static BUTTONSSTRUCT _joybtn    ;

static bool                _bJoypad   ;
static int                 _ok_count  ;
static BUTTONSCONFIGSTRUCT _key_config;

static ListFrame*          _lf_title ;
static ListFrame*          _lf_config;
static ListFrame*          _lf_cancel;

static SIMPLELIST          _list_title       ;
static SIMPLELIST          _lists_config[ 6 ];
static SIMPLELIST          _lists_cancel[ 2 ];

static const SIMPLELIST lists_config[ 6 ] =
{
	{ 0, "←......[ 　 ]", NULL, false },
	{ 0, "→......[ 　 ]", NULL, false },
	{ 0, "↑......[ 　 ]", NULL, false },
	{ 0, "↓......[ 　 ]", NULL, false },
	{ 0, "Ｏk.....[ 　 ]", NULL, false },
	{ 0, "Ｃancel.[ 　 ]", NULL, false },
};

static const SIMPLELIST lists_cancel[ 2 ] =
{
	{ 0, "CANCEL : ESC", NULL, false },
	{ 0, "ACCEPTED!"   , NULL, false },
};

static BOOL _ActionKeyConfig_Break_Keyboard( BUTTONSCONFIGSTRUCT *p_conf )
{
	long key_code, i;

	_ani_wait++;

	for( key_code = 0; key_code <  0x100; key_code++ )
	{
		if( KeyControl_IsKeyTrigger( key_code ) ) break;
	}

	if( key_code == 0x100 ) return FALSE;

	for( i = 0; i < _pos; i++ )
	{
		if( p_conf->key_codes[ i ] == key_code ) break;
	}

	if( i < _pos )
	{
		_pos = 0;
		Buttons_GetConfigDefault_Keyboard( p_conf );
		SEFF_Voice_Play( SeNo_Cancel );
		return FALSE;
	}

	p_conf->key_codes[ _pos ] = (unsigned char)key_code;
	SEFF_Voice_Play( SeNo_Ok );

	if( _pos < enum_Buttons_KeyOn_NUM - 1 )
	{
		_pos++;
		return FALSE;
	}

	return TRUE;
}

// タイトル画面で押したボタン判定をトリガとしないように。
static BOOL _Joypad_ClearTrigger( void )
{
	DXJOYPADSTRUCT joypad;
	if( !DxInput_Get( &joypad ) ) return FALSE;
	_joybtn.btn = joypad.button_bits;
	Buttons_UpdateTriggers( &_joybtn );
	return TRUE;
}

static BOOL _ActionKeyConfig_Break_Joypad( BUTTONSCONFIGSTRUCT *p_conf )
{
	DXJOYPADSTRUCT joypad;
	long           button;
	long           i;

	_ani_wait++;

	if( !DxInput_Get( &joypad ) ) return FALSE;

	_joybtn.btn = joypad.button_bits;
	Buttons_UpdateTriggers( &_joybtn );

	for( button = 0; button <  MAX_JOYPADBUTTON; button++ )
	{
		if( _joybtn.trg & ( 1 << button ) ) break;
	}

	if( button == MAX_JOYPADBUTTON ) return FALSE;

	for( i = 0; i < _pos; i++ )
	{
		if( p_conf->joy_codes[ i ] == button ) break;
	}

	if( i < _pos )
	{
		_pos = 0;
		Buttons_GetConfigDefault_Joypad( p_conf );
		SEFF_Voice_Play( SeNo_Cancel );
		return FALSE;
	}

	p_conf->joy_codes[ _pos ] = (unsigned char)button;
	SEFF_Voice_Play( SeNo_Ok );

	if( _pos < enum_Buttons_JoyOn_NUM - 1 )
	{
		_pos++;
		return FALSE;
	}

	return TRUE;
}


static void _PutKeyConfig( const RECT *p_rcview, const BUTTONSCONFIGSTRUCT *p_conf, BOOL bJoypad )
{
	RECT rcCursor[] =
	{
		{ 8, 0,16, 8},
		{ 0, 0, 0, 0},
	};

	RECT rc;

	long cursor_x = 104;
	long cursor_y = 24 + 2;

	DxDraw_Put_Clip( p_rcview, cursor_x, cursor_y + _pos * 12, &rcCursor[ (_ani_wait/4)%2 ], SfNo_KEYCODE );
	

	if( bJoypad )
	{
		for( long i = 0; i < enum_Buttons_JoyOn_NUM; i++ )
		{
			rc.left   = ( p_conf->joy_codes[ i ] + 1 ) * 8;
			rc.top    = 24;
			rc.right  = rc.left + 8;
			rc.bottom = rc.top  + 8;
			DxDraw_Put_Clip( p_rcview, cursor_x, cursor_y + i * 12, &rc, SfNo_KEYCODE );
		}
	}
	else
	{
		for( long i = 0; i <  enum_Buttons_KeyOn_NUM; i++ )
		{
			rc.left   = ( p_conf->key_codes[ i ] % 16 ) * 8;
			rc.top    = ( p_conf->key_codes[ i ] / 16 ) * 8;
			rc.right  = rc.left + 8;
			rc.bottom = rc.top  + 8;
			DxDraw_Put_Clip( p_rcview, cursor_x, cursor_y + i * 12, &rc, SfNo_KEYCODE );
		}
	}
}

void scene_Buttons_Initialize( void )
{
	memcpy( _lists_config, lists_config, sizeof(SIMPLELIST) * 6 );
	memcpy( _lists_cancel, lists_cancel, sizeof(SIMPLELIST) * 2 );

	_list_title.p_buf =  NULL       ;
	_list_title.p_str = "KEY CONFIG";
	_list_title.param =            1;

	_bJoypad   = false;

	// title
	_lf_title  = new ListFrame( LISTFRAME_STATIC, 10, 1, POSFROM_LT, 12,  8, false );
	_lf_title->List_Set( &_list_title, 1 );

	// config
	_lf_config = new ListFrame( LISTFRAME_STATIC, 15, 6, POSFROM_LT, 48, 24, false );

	// cancel
	_lf_cancel = new ListFrame( LISTFRAME_STATIC, 10, 1, POSFROM_LB, 12,  8, false );
	_lf_cancel->List_Set( _lists_cancel, 2 );
}
void scene_Buttons_Release( void )
{
	if( _lf_title  ) delete _lf_title ;
	if( _lf_config ) delete _lf_config;
	if( _lf_cancel ) delete _lf_cancel;
}


void scene_Buttons_Set( void )
{
	_bJoypad = Buttons_IsJoypad();
	if( _bJoypad ){ _lf_config->List_Set( &_lists_config[ 4 ], 2 ); }
	else          { _lf_config->List_Set(  _lists_config     , 6 ); }
	_lf_cancel->List_Set_CursorY( 0 );

	if( !Buttons_LoadConfig( &_key_config ) ) Buttons_GetConfigDefault( &_key_config );

	memset( &_joybtn, 0, sizeof(BUTTONSSTRUCT) );
	_pos      = 0;
	KeyControl_Clear();
	_ok_count = 0;

	if( _bJoypad ) _Joypad_ClearTrigger();
}



void scene_Buttons_Procedure( void )
{
	if( _ok_count )
	{
		if( ++_ok_count > 60 )
		g_scene->Reset_Set();
	}
	else
	{
		// escape
		KeyControl_UpdateTrigger();
		if( KeyControl_IsKeyTrigger( VK_ESCAPE ) ) g_scene->Reset_Set();

		if( !KeyControl_IsKeyTrigger( VK_F1 ) && !KeyControl_IsKeyTrigger( VK_F2 ) )
		{
			if( _bJoypad ){ if( _ActionKeyConfig_Break_Joypad(   &_key_config ) ) _ok_count = 1; }
			else          { if( _ActionKeyConfig_Break_Keyboard( &_key_config ) ) _ok_count = 1; }
		}

		if( _ok_count )
		{
			Buttons_SaveConfig( &_key_config );
			_lf_cancel->List_Set_CursorY( 1 );
		}
	}

	const RECT* rc_view = &g_rc_wnd;

	DxDraw_PaintRect( &g_rc_wnd, g_color_bg_text );
	_lf_title ->Put( rc_view );
	_lf_config->Put( rc_view );
	_lf_cancel->Put( rc_view );
	_PutKeyConfig(   &g_rc_wnd, &_key_config, _bJoypad );
}