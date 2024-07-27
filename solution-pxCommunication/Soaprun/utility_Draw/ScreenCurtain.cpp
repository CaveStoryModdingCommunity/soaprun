#include <StdAfx.h>

#include "../../Dx/DxDraw.h"

#include "ScreenCurtain.h"

void ScreenCurtain::Mask()
{
	_action          = _CURTAINACT_MASK;
	_rc_num          = 1;
	_rcs[ 0 ].left   = 0;
	_rcs[ 0 ].top    = 0;
	_rcs[ 0 ].right  = FIELDVIEW_W      ;
	_rcs[ 0 ].bottom = FIELDVIEW_H      ;
}

ScreenCurtain::ScreenCurtain( DWORD color )
{
	memset( _rcs, 0, sizeof(RECT) * _MAX_CURTAINRECT );
	_action = _CURTAINACT_NONE;
	_rc_num = 0;
	_color  = color;
}

void ScreenCurtain::Set_In        (){
	_action = _CURTAINACT_IN        ; _rc_num =  4; _count = 0; _max_count =16; }
void ScreenCurtain::Set_Out_Side  (){
	_action = _CURTAINACT_OUT_SIDE  ; _rc_num =  4; _count = 0; _max_count =16; }
void ScreenCurtain::Set_Out_Center(){
	_action = _CURTAINACT_OUT_CENTER; _rc_num =  1; _count = 0; _max_count =16; }

void ScreenCurtain::Procedure()
{
	int half_w = FIELDVIEW_W / 2;
	int half_h = FIELDVIEW_H / 2;


	switch( _action )
	{
	case _CURTAINACT_IN        :
	case _CURTAINACT_OUT_SIDE  :
	case _CURTAINACT_OUT_CENTER: _count++; break;
	}

	
	switch( _action )
	{
	case _CURTAINACT_NONE: break;

	case _CURTAINACT_IN  :

		int rev_count;
		int w, h     ;

		rev_count = _max_count - _count;
		w         = (FIELDVIEW_W / 2) * rev_count / _max_count;
		h         = (FIELDVIEW_H / 2) * rev_count / _max_count;

		_rcs[ 0 ].left   = 0;
		_rcs[ 0 ].top    = 0;
		_rcs[ 0 ].right  = 0          + w;
		_rcs[ 0 ].bottom = FIELDVIEW_H;

		_rcs[ 1 ].left   = FIELDVIEW_W - w;
		_rcs[ 1 ].top    = 0;
		_rcs[ 1 ].right  = FIELDVIEW_W;
		_rcs[ 1 ].bottom = FIELDVIEW_H;

		_rcs[ 2 ].left   = 0;
		_rcs[ 2 ].top    = 0;
		_rcs[ 2 ].right  = FIELDVIEW_W;
		_rcs[ 2 ].bottom = 0          + h;

		_rcs[ 3 ].left   = 0;
		_rcs[ 3 ].top    = FIELDVIEW_H - h;
		_rcs[ 3 ].right  = FIELDVIEW_W;
		_rcs[ 3 ].bottom = FIELDVIEW_H;

		if( _count >= _max_count ){ _action = _CURTAINACT_NONE; _rc_num = 0; }
		break;
		
	case _CURTAINACT_OUT_SIDE:

		_count++;

		_rcs[ 0 ].left   = 0;
		_rcs[ 0 ].top    = 0;
		_rcs[ 0 ].right  = half_w * _count / _max_count;//half_w - half_w / _count;
		_rcs[ 0 ].bottom = FIELDVIEW_H;

		_rcs[ 1 ].left   = FIELDVIEW_W - half_w * _count / _max_count; //half_w + half_w / _count;
		_rcs[ 1 ].top    = 0;
		_rcs[ 1 ].right  = FIELDVIEW_W;
		_rcs[ 1 ].bottom = FIELDVIEW_H;

		_rcs[ 2 ].left   = 0;
		_rcs[ 2 ].top    = 0;
		_rcs[ 2 ].right  = FIELDVIEW_W;
		_rcs[ 2 ].bottom = half_h * _count / _max_count;//half_h - half_h / _count;
		
		_rcs[ 3 ].left   = 0;
		_rcs[ 3 ].top    = FIELDVIEW_H - half_h * _count / _max_count;//half_h + half_h / _count;
		_rcs[ 3 ].right  = FIELDVIEW_W;
		_rcs[ 3 ].bottom = FIELDVIEW_H;

		if( _count >= _max_count ) Mask();
		break;

	case _CURTAINACT_OUT_CENTER:
		_count++;
		_rcs[ 0 ].left   = half_w - half_w * _count / _max_count;
		_rcs[ 0 ].top    = half_h - half_h * _count / _max_count;
		_rcs[ 0 ].right  = half_w + half_w * _count / _max_count;
		_rcs[ 0 ].bottom = half_h + half_h * _count / _max_count;

		if( _count >= _max_count ) Mask();
		break;

	case _CURTAINACT_MASK: break;
	}
}

void ScreenCurtain::Put()
{
	for( int i = 0; i < _rc_num; i++ )
	{
		DxDraw_PaintRect( &_rcs[ i ], _color );
	}
}

bool ScreenCurtain::IsIdle()
{
	if( _action == _CURTAINACT_NONE || _action == _CURTAINACT_MASK )
	{
		return true;
	}
	return false;
}