#include <StdAfx.h>

#include "../../Dx/DxDraw.h"

#include "Pane.h"


void Pane::_Zero()
{
	_b_visible = false;
	_type      = _PANETYPE_None;
	_x         = 0;
	_y         = 0;
	_w         = 0;
	_h         = 0;

	_color     = 0;
	_surf_no   = 0;

	_ani.type  = _PANEANI_None;
	_ani.x     = 0;
	_ani.y     = 0;
	_ani.count = 0;
	_ani.c_    = 0;
	_ani.wait  = 0;
	_ani.w_    = 0;
	_max_child = 0;
	_children  = NULL;
}


Pane::Pane()
{
	_Zero();
}

Pane::Pane( int max_child )
{
	_Zero();
	if( max_child > 0 )
	{
		_children = (Pane**)malloc( sizeof(Pane*) * max_child );
		memset( _children, 0,       sizeof(Pane*) * max_child );
		_max_child = max_child;
	}
}

Pane::~Pane()
{
	if( _children ) free( _children );
}

void Pane::SetType_View( int x, int y, int w, int h )
{
	_type = _PANETYPE_View;
	_b_visible = true;

	_x = x; _y = y; _w = w; _h = h;
}

void Pane::SetType_Fill ( int x, int y, int w, int h, DWORD color )
{
	_type = _PANETYPE_Fill;
	_b_visible = true;
	_x = x; _y = y; _w = w; _h = h;
	_color = color;
}

void Pane::SetType_Image( int x, int y, int w, int h, int surf_no )
{
	_type = _PANETYPE_Image;
	_b_visible = true;
	_x = x; _y = y; _w = w; _h = h;
	_surf_no = surf_no;
}

void Pane::SetType_Frame( int w, int h, DWORD bk_color, int sf_h, int sf_v )
{
	_type = _PANETYPE_Frame;
	_b_visible = true;
	_x = 0; _y = 0; _w = w; _h = h;
	_sf_v    = sf_v    ;
	_sf_h    = sf_h    ;
	_color   = bk_color;
}


void Pane::Visible( bool b )
{
	_b_visible = b;
//	_type = _PANETYPE_Vanish;
}


bool Pane::Child_Add( Pane *child )
{
	for( int c = 0; c < _max_child; c++ )
	{
		if( !_children[ c ] )
		{
			_children[ c ] = child; return true;
		}
	}
	return false;
}

bool Pane::Child_Insert( Pane *child )
{
	for( int c = 0; c < _max_child; c++ )
	{
		if( !_children[ c ] )
		{
			for( c; c > 0; c-- ) _children[ c ] = _children[ c - 1 ];
			_children[ 0 ] = child;
			return true;
		}
	}
	return false;
}

bool Pane::Child_Remove( Pane *child )
{
	for( int c = 0; c < _max_child; c++ )
	{
		if( _children[ c ] == child )
		{
			_children[ c ] = NULL;
			c++;
			for( c; c < _max_child; c++ )
			{
				_children[ c - 1 ] = _children[ c ];
				_children[ c     ] = NULL;
			}
			return true;
		}
	}
	return false;
}

int  Pane::W() const { return _w; }
int  Pane::H() const { return _h; }

void Pane::Put( const RECT *rc_parent, int x, int y ) const
{
//	if( _type == _PANETYPE_Vanish ) return;
	if( !_b_visible ) return;

	RECT rc;

	x += _x;
	y += _y;

	rc.left = x; rc.right  = x + _w;
	rc.top  = y; rc.bottom = y + _h;
	if( rc.left   < rc_parent->left   ) rc.left   = rc_parent->left  ;
	if( rc.top    < rc_parent->top    ) rc.top    = rc_parent->top   ;
	if( rc.right  > rc_parent->right  ) rc.right  = rc_parent->right ;
	if( rc.bottom > rc_parent->bottom ) rc.bottom = rc_parent->bottom;
	
	switch( _type )
	{
	case _PANETYPE_View   :
		break;

	case _PANETYPE_Fill   :
		DxDraw_PaintRect_Clip( rc_parent, &rc, _color );
		break;
	
	case _PANETYPE_Image:
		RECT rc_surf;
		rc_surf.left   = _w * ( _ani.x + _ani.c_ );
		rc_surf.top    = _h * ( _ani.y           );
		rc_surf.right  = rc_surf.left + _w;
		rc_surf.bottom = rc_surf.top  + _h; 
		DxDraw_Put_Clip( rc_parent, x, y, &rc_surf, _surf_no );
		break;


	case _PANETYPE_Frame:

		RECT rc_sf;
		{
			rc_sf.left  = rc.left  + _PANEFRAME_W;
			rc_sf.top   = rc.top   + _PANEFRAME_H;
			rc_sf.right = rc.right - _PANEFRAME_W;
			rc_sf.bottom = rc.bottom  - _PANEFRAME_H;
			DxDraw_PaintRect_Clip( rc_parent, &rc_sf, _color );
		}

		{ // è„
			rc_sf.top = 0;
			rc_sf.bottom   = _PANEFRAME_H;

			rc_sf.left     = 0;
			rc_sf.right    = _w - _PANEFRAME_W;

			DxDraw_Put_Clip( rc_parent, rc.left, rc.top, &rc_sf, _sf_h );

			rc_sf.left   = _PANEFRAMEIMAGE_W - _PANEFRAME_W;
			rc_sf.right  = _PANEFRAMEIMAGE_W;

			DxDraw_Put_Clip( rc_parent, rc.left + _w - _PANEFRAME_W, rc.top, &rc_sf, _sf_h );
		}

		{ // â∫
			rc_sf.top      = _PANEFRAME_H;
			rc_sf.bottom   = _PANEFRAME_H * 2;

			rc_sf.left     = 0;
			rc_sf.right    = _w - _PANEFRAME_W;

			DxDraw_Put_Clip( rc_parent, rc.left, rc.bottom - _PANEFRAME_H, &rc_sf, _sf_h );

			rc_sf.left   = _PANEFRAMEIMAGE_W - _PANEFRAME_W;
			rc_sf.right  = _PANEFRAMEIMAGE_W;

			DxDraw_Put_Clip( rc_parent, rc.left + _w - _PANEFRAME_W, rc.bottom - _PANEFRAME_H, &rc_sf, _sf_h );
		}

		{ // ç∂Ç∆âE
			rc_sf.top      = _PANEFRAME_H;
			rc_sf.bottom   = _h - _PANEFRAME_H;

			rc_sf.left     = 0;
			rc_sf.right    = _PANEFRAME_W;

			DxDraw_Put_Clip( rc_parent, rc.left, rc.top + _PANEFRAME_H, &rc_sf, _sf_v );

			rc_sf.left     = _PANEFRAME_W;
			rc_sf.right    = _PANEFRAME_W * 2;

			DxDraw_Put_Clip( rc_parent, rc.left + _w - _PANEFRAME_W, rc.top + _PANEFRAME_H, &rc_sf, _sf_v );
		}
		break;
	default:return;
	}
	
	for( int c = 0; c < _max_child; c++ )
	{
		if( _children[ c ] ) _children[ c ]->Put( &rc, x, y );
	}
}

void Pane::SetAnimation( int x, int y, int count, int wait )
{
	_b_visible = true;
	_type      = _PANETYPE_Image   ;
	_ani.type  = _PANEANI_Animation;
	_ani.x     = x    ;
	_ani.y     = y    ;
	_ani.count = count;
	_ani.wait  = wait ;

	_ani.c_    =     0;
	_ani.w_    =     0;
}

void Pane::SetAnimationY( int y )
{
	_b_visible = true;
	_ani.y     = y    ;
}

void Pane::SetScrollOut( int dir_x, int dir_y )
{
	_x = 0;
	_y = 0;
	_ani.type  = _PANEANI_ScrollOut;
	_ani.x     = dir_x;
	_ani.y     = dir_y;
}

void Pane::SetScrollIn( int dir_x, int dir_y )
{
	if(      dir_x < 0 ) _x =  _w;
	else if( dir_x > 0 ) _x = -_w;
	if(      dir_y < 0 ) _y =  _h;
	else if( dir_y > 0 ) _y = -_h;
	_ani.type  = _PANEANI_ScrollIn;
	_ani.x     = dir_x;
	_ani.y     = dir_y;
}


void Pane::Animation()
{
	if( !_b_visible ) return;
//	if( _type == _PANETYPE_Vanish ) return;

	bool b;

	switch( _ani.type )
	{
	case _PANEANI_Animation:

		if( !_ani.count ) break;
		if( ++_ani.w_ >= _ani.wait ){ _ani.w_ = 0; if( ++_ani.c_ >= _ani.count ) _ani.c_ = 0; }
		break;

	case _PANEANI_ScrollOut:
		_x += _ani.x;
		_y += _ani.y;

		b = false;

		if( _ani.x < 0 && _x <= -_w ) b = true;
		if( _ani.x > 0 && _x >=  _w ) b = true;
		if( _ani.y < 0 && _y <= -_h ) b = true;
		if( _ani.y > 0 && _y >=  _h ) b = true;
		if( !b ) break;

		_ani.type = _PANEANI_None;

		break;

	case _PANEANI_ScrollIn:

		_x += _ani.x;
		_y += _ani.y;

		b = false;

		if( _ani.x < 0 && _x <= 0 ) b = true;
		if( _ani.x > 0 && _x >= 0 ) b = true;
		if( _ani.y < 0 && _y <= 0 ) b = true;
		if( _ani.y > 0 && _y >= 0 ) b = true;
		if( !b ) break;

		_ani.type = _PANEANI_None;


		break;
	}

	for( int c = 0; c < _max_child; c++ )
	{
		if( _children[ c ] ) _children[ c ]->Animation();
	}
}

bool Pane::IsAnimation()
{
	if( !_b_visible ) return false;
//	if( _type     == _PANETYPE_Vanish ) return false;
	if( _ani.type == _PANEANI_None    ) return false;
	return true;
}

void Pane::SetPos( int x, int y )
{
	_x = x;
	_y = y;
}

void Pane::SetSurface( int surf_no )
{
	_surf_no = surf_no;
}

void Pane::SetAnimationGap( short gap )
{
	_ani.w_ = gap;
	for( int c = 0; c < _max_child; c++ )
	{
		if( _children[ c ] ) _children[ c ]->SetAnimationGap( gap );
	}
}

int Pane::GetTotal()
{
	int count = 1;
	for( int c = 0; c < _max_child; c++ )
	{
		if( _children[ c ] && _children[ c ]->_b_visible ) count += _children[ c ]->GetTotal();
	}
	return count;
}
