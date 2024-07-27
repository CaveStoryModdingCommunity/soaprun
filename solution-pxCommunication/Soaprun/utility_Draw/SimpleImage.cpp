#include <StdAfx.h>

#include "../../Dx/DxDraw.h"     // Direct Draw

#include "SimpleImage.h"
#include "PxImage.h"

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

SimpleImage::SimpleImage( void )
{
	_surface_no = 0;
}

SimpleImage::~SimpleImage( void )
{
	if( _surface_no ) DxDraw_SubSurface_Release( _surface_no );
}

bool SimpleImage::Ready( int w, int h, int surface_no )
{
	if( !DxDraw_SubSurface_ReadyBlank( w, h, surface_no, false ) ) return false;
	_surface_no = surface_no;
	return true;
}

bool SimpleImage::Reload( const char* name, bool b_exterior, int ani_num )
{
	bool b_ret = false;
	long w, h;

	if( !PxImage_Reload( name, _surface_no, b_exterior ) ) goto End;
	DxDraw_SubSurface_GetImageSize( &w, &h, _surface_no );

	W        = w / ani_num;
	H        = h          ;
	_ani_num =     ani_num;
	_ani_no  =           0;
	b_ret    = true       ;
End:
	return b_ret;
}

void SimpleImage::Put( const RECT* rc_view )
{
	RECT rc;
	int  x = _x - W / 2;
	int  y = _y - H / 2;

	rc.top   = 0; rc.bottom = H;
	rc.left  = _ani_no * W;
	rc.right = rc.left + W;

	DxDraw_Put_Clip( rc_view, x + _off_x, y + _off_y, &rc, _surface_no );
}

void SimpleImage::SetPosition( int x, int y )
{
	_x        = x;
	_y        = y;
	_ani_wait = _ani_no = _off_x = _off_y = 0;
	_action   = SIMPLEIMAGE_STOP;

}

void SimpleImage::SetAction( SIMPLEIMAGEACTION action )
{
	_action   = action;
	_ani_wait = _ani_no = _off_x = _off_y = 0;
}

void SimpleImage::Action( void )
{
	switch( _action )
	{
	case SIMPLEIMAGE_STOP : _ani_no = _ani_wait = 0; break;

	case SIMPLEIMAGE_FLASH:
		_ani_no = (_ani_wait/2) % 2;
		_ani_wait++;
		if( _ani_wait >= _SHOCKANIMATIONNUM ) _action = SIMPLEIMAGE_STOP;
		break;

	case SIMPLEIMAGE_SHOCK:
		_ani_wait++;
		_off_x = _shock_tbl[ _ani_wait ].x;
		_off_y = _shock_tbl[ _ani_wait ].y;
		if( _ani_wait >= _SHOCKANIMATIONNUM )
		{
			_action = SIMPLEIMAGE_STOP;
		}
		break;
	}
}
