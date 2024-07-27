#include <StdAfx.h>

#define MAX_KEY     0x100
#define CLICK_LEFT  0x01
#define CLICK_RIGHT 0x10

//トリガの生成
static char _key[    MAX_KEY] = {0};
static char _key_old[MAX_KEY] = {0};
static char _key_trg[MAX_KEY] = {0};
static long _click            =  0;
static long _click_old        =  0;
static long _click_trg        =  0;
static long _wheel            =  0;
static long _wheel_old        =  0;
static long _wheel_trg        =  0;

static BOOL _bCapture         = FALSE;

void KeyControl_Clear( void )
{
	memset( _key,     0, MAX_KEY );
	memset( _key_old, 0, MAX_KEY );
	memset( _key_trg, 0, MAX_KEY );
	_click      = 0;
	_click_old  = 0;
	_click_trg  = 0;
	_wheel      = 0;
	_wheel_old  = 0;
	_wheel_trg  = 0;
}

void KeyControl_WM_MESSAGE( HWND hWnd, UINT msg, WPARAM w )
{
	switch( msg )
	{

	// キーが 押された/離された
	case WM_KEYDOWN    : _key[ w ] = 1; break;
	case WM_KEYUP      : _key[ w ] = 0; break;

	// マウス
	case WM_LBUTTONDOWN: _click |=  CLICK_LEFT ; if( !_bCapture                ){ _bCapture = TRUE ; SetCapture( hWnd ); } break;
	case WM_RBUTTONDOWN: _click |=  CLICK_RIGHT; if( !_bCapture                ){ _bCapture = TRUE ; SetCapture( hWnd ); } break;
	case WM_LBUTTONUP  : _click &= ~CLICK_LEFT ; if( !( _click & CLICK_RIGHT ) ){ _bCapture = FALSE; ReleaseCapture(  ); } break;
	case WM_RBUTTONUP  : _click &= ~CLICK_RIGHT; if( !( _click & CLICK_LEFT  ) ){ _bCapture = FALSE; ReleaseCapture(  ); } break;
	}
}
void KeyControl_WM_MOUSEWHEEL( HWND hWnd, WPARAM w )
{
	if( (short)HIWORD(w) > 0 ) _wheel++;
	if( (short)HIWORD(w) < 0 ) _wheel--;
}

void KeyControl_UpdateTrigger( void )
{
	long i;

	// キーボードのトリガを取得
	for( i = 0; i < MAX_KEY; i++ )
	{
		if( !_key_old[i] && _key[i] ) _key_trg[i] = 1;
		else                          _key_trg[i] = 0;
		_key_old[i] = _key[i];
	}

	// マウスのトリガを取得
	_click_trg = _click ^ _click_old;
	_click_trg = _click & _click_trg;
	_click_old = _click;

	// ホイールトリガ
	if(      _wheel < _wheel_old ) _wheel_trg = -1;
	else if( _wheel > _wheel_old ) _wheel_trg =  1;
	else                           _wheel_trg =  0;
	_wheel_old = _wheel;
}

long KeyControl_GetWheel( void ){ return _wheel_trg; }

BOOL KeyControl_IsKey( long vk )
{
	if( _key[ vk ] ) return TRUE;
	return FALSE;
}

BOOL KeyControl_IsKeyTrigger( long vk )
{
	if( _key_trg[ vk ] ) return TRUE;
	return FALSE;
}

BOOL KeyControl_IsClickLeft( void )
{
	if( _click & CLICK_LEFT ) return TRUE;
	return FALSE;
}

BOOL KeyControl_IsClickRight( void )
{
	if( _click & CLICK_RIGHT ) return TRUE;
	return FALSE;
}

BOOL KeyControl_IsClickLeftTrigger( void )
{
	if( _click_trg & CLICK_LEFT ) return TRUE;
	return FALSE;
}

BOOL KeyControl_IsClickRightTrigger( void )
{
	if( _click_trg & CLICK_RIGHT ) return TRUE;
	return FALSE;
}

