#include <StdAfx.h>

#include "../../Dx/DxDraw.h"

#include "GenericFrame.h"

#define _SHOCKANIMATIONNUM 10

static POINT _shock_tbl[ _SHOCKANIMATIONNUM ] =
{
	{ 0,-3},
	{ 0, 3},
	{ 0,-2},
	{ 0, 2},
	{ 0,-2},
	{ 0, 2},
	{ 0,-1},
	{ 0, 1},
	{ 0, 0},
	{ 0, 1},
};


GenericFrame::GenericFrame( const RECT* p_rect )
{
	if( p_rect ) _rect = *p_rect;
	else         memset( &_rect, 0, sizeof(RECT) );
	_back_color = DxDraw_GetSurfaceColor( SfNo_FRAME_B );
	_action     = GENERICFRAME_STOP;
	_ani_wait   = _ani_no = _off_x = _off_y = 0;
}
/*
GenericFrame::GenericFrame( void )
{
	RECT rect;
	memset( &rect, 0, sizeof(RECT) );
	GenericFrame( &rect );
}
*/

void GenericFrame::SetRect( const RECT* p_rect )
{
	_action   = GENERICFRAME_STOP;
	_ani_wait = _ani_no = _off_x = _off_y = 0;
	_rect     = *p_rect;
}

void GenericFrame::SetAction( GENERICFRAMEACTION action )
{
	_action   = action;
	_ani_wait = _ani_no = _off_x = _off_y = 0;
}

void GenericFrame::Action( void )
{
	switch( _action )
	{
	case GENERICFRAME_STOP : _ani_no = _ani_wait = 0; break;

	case GENERICFRAME_FLASH:
		_ani_no = (_ani_wait/2) % 2;
		_ani_wait++;
		if( _ani_wait >= _SHOCKANIMATIONNUM ) _action = GENERICFRAME_STOP;
		break;

	case GENERICFRAME_SHOCK:
		_ani_wait++;
		_off_x = _shock_tbl[ _ani_wait ].x;
		_off_y = _shock_tbl[ _ani_wait ].y;
		if( _ani_wait >= _SHOCKANIMATIONNUM )
		{
			_action = GENERICFRAME_STOP;
		}
		break;
	}
}

void GenericFrame::Put( const RECT* rc_view )
{
	RECT rc;
	int  x, y;

	{
		rc = _rect;
		rc.left   += GENERICFRAME_W;
		rc.right  -= GENERICFRAME_W;
		rc.top    += GENERICFRAME_H;
		rc.bottom -= GENERICFRAME_H;
		DxDraw_PaintRect( &rc, _back_color );
	}

	{ // è„
		rc.top    = 0;
		rc.bottom = GENERICFRAME_H;

		rc.left   = 0;
		rc.right  = rc.left + _rect.right - _rect.left - GENERICFRAME_W;
		x         = _rect.left                    + _off_x;
		y         = _rect.top                     + _off_y;
		DxDraw_Put_Clip( rc_view, x , y, &rc, SfNo_FRAME_H );
		rc.left   = FRAMEIMAGE_H_W - GENERICFRAME_W;
		rc.right  = FRAMEIMAGE_H_W;
		x         = _rect.right  - GENERICFRAME_W + _off_x;
		y         = _rect.top                     + _off_y;
		DxDraw_Put_Clip( rc_view, x, y, &rc, SfNo_FRAME_H );
	}

	{ // â∫
		rc.top    = GENERICFRAME_H;
		rc.bottom = GENERICFRAME_H * 2;

		rc.left   = 0;
		rc.right  = rc.left + _rect.right - _rect.left - GENERICFRAME_W;
		x         = _rect.left                    + _off_x;
		y         = _rect.bottom - GENERICFRAME_H + _off_y;
		DxDraw_Put_Clip( rc_view, x, y, &rc, SfNo_FRAME_H );
		rc.left   = FRAMEIMAGE_H_W - GENERICFRAME_W;
		rc.right  = FRAMEIMAGE_H_W;
		x         = _rect.right  - GENERICFRAME_W + _off_x;
		y         = _rect.bottom - GENERICFRAME_H + _off_y;
		DxDraw_Put_Clip( rc_view, x, y, &rc, SfNo_FRAME_H );
	}

	{ // ç∂Ç∆âE
		rc.top    = 0;
		rc.bottom = _rect.bottom - _rect.top - GENERICFRAME_H * 2;
		rc.left   = 0;
		rc.right  = GENERICFRAME_W;
		x         = _rect.left                    + _off_x;
		y         = _rect.top    + GENERICFRAME_H + _off_y;
		DxDraw_Put_Clip( rc_view, x, y, &rc, SfNo_FRAME_V );
		rc.left   = GENERICFRAME_W;
		rc.right  = GENERICFRAME_W*2;
		x         = _rect.right  - GENERICFRAME_W + _off_x;
		y         = _rect.top    + GENERICFRAME_H + _off_y;
		DxDraw_Put_Clip( rc_view, x, y, &rc, SfNo_FRAME_V );
	}
}
