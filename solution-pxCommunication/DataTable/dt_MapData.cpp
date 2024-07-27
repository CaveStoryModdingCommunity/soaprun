#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "dt_MapData.h"

#define _MAX_MAPWIDTH  32767
#define _MAX_MAPLENGTH 32767


static BOOL _malloc_zero( void **pp, long size )
{
	*pp = malloc( size ); if( !( *pp ) ) return FALSE;
	memset( *pp, 0, size );              return TRUE;
}

static void _free( void **pp )
{
	if( *pp ){ free( *pp ); *pp = NULL; }
}


/////////////////
// グローバル
/////////////////

dt_MapData::dt_MapData( int max_w, int max_l, int surf_mparts )
{
	_p           = NULL;
	_max_w       = max_w;
	_max_l       = max_l;
	_w           = 0;
	_l           = 0;
	_surf_mparts = surf_mparts;
//	_att_func    = NULL;

	if( max_w > _MAX_MAPWIDTH  ) return;
	if( max_l > _MAX_MAPLENGTH ) return;
	if( !_malloc_zero( (void**)&_p, max_w * max_l ) ) return;

	memset( &location, 0, sizeof(MDLOCATION) );
}

dt_MapData::~dt_MapData()
{
	_free( (void**)&_p );
	_max_w = 0;
	_max_l = 0;
	_w     = 0;
	_l     = 0;
}

void dt_MapData::Clear( void )
{
	if( _p && _max_w && _max_l )
	{
		memset( _p, 0, _max_w * _max_l );
		_w     = 0;
		_l     = 0;
	}
}


void dt_MapData::SetParts( int x, int y, unsigned char parts )
{
	if( x >= _w ) return;
	if( y >= _l ) return;
	_p[ y * _max_w + x ] = parts;
}

unsigned char dt_MapData::GetParts( int x, int y )
{
	if( x >= _w ) return 0;
	if( y >= _l ) return 0;
	return _p[ y * _max_w + x ];
}

bool dt_MapData::SetSize( int w, int l, int align_flags )
{
	if( w > _max_w ) return false;
	if( l > _max_l ) return false;

	int offset_x = 0;
	int offset_y = 0;
	if( align_flags & MAPALIGNFLAG_RIGHT  ) offset_x = w - _w;
	if( align_flags & MAPALIGNFLAG_BOTTOM ) offset_y = l - _l;

	// フレーム外を掃除
	for( int y = 0; y < _max_l; y++ )
	{
		for( int x = 0; x < _max_w; x++ )
		{
			if( x >= _w || y >= _l ) _p[ y * _max_w + x ] = 0;
		}
	}

	int x_dst;
	int y_dst;

	if( offset_x < 0 )
	{
		for( int y_src = 0; y_src < _l; y_src++ )
		{
			for( int x_src = 0; x_src < _w; x_src++ )
			{
				x_dst = x_src + offset_x;
				if( x_dst >= 0 )
				{
					_p[ y_src * _max_w + x_dst ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}
	else if( offset_x > 0 )
	{
		for( int y_src = 0; y_src < _l; y_src++ )
		{
			for( int x_src = _w - 1; x_src >= 0; x_src-- )
			{
				x_dst = x_src + offset_x;
				if( x_dst < _max_w )
				{
					_p[ y_src * _max_w + x_dst ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}

	if( offset_y < 0 )
	{
		for( int y_src = 0; y_src < _l; y_src++ )
		{
			y_dst = y_src + offset_y;
			if( y_dst >= 0 )
			{
				for( int x_src = 0; x_src < _w; x_src++ )
				{
					_p[ y_dst * _max_w + x_src ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}
	else if( offset_y > 0 )
	{
		for( int y_src = _l - 1; y_src >= 0; y_src-- )
		{
			y_dst = y_src + offset_y;
			if( y_dst < _max_l )
			{
				for( int x_src = 0; x_src < _w; x_src++ )
				{
					_p[ y_dst * _max_w + x_src ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}

	_w = w;
	_l = l;

	return true;
}

bool dt_MapData::Read( FILE *fp )
{
	bool  b_ret = false;
	unsigned short us;

	Clear();

	if( fread( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End; _w = us;
	if( fread( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End; _l = us;

	if( _w > _max_w || _l > _max_l ) goto End;

	for( int y = 0; y < _l; y++ )
	{
		if( fread( &_p[ y * _max_w ], sizeof(unsigned char), _w, fp ) != _w ) goto End;
	}

	b_ret = true;
End:

	if( !b_ret ) Clear();

	return b_ret;
}

bool dt_MapData::Write( FILE* fp )
{
	bool           b_ret  = false;
	unsigned short us;

	us = (unsigned short)_w;
	if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;

	us = (unsigned short)_l;
	if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;

	for( int y = 0; y < _l; y++ )
	{
		if( fwrite( &_p[ y * _max_w ], sizeof(unsigned char), _w, fp ) != _w ) goto End;
	}

	b_ret = true;
End:

	return b_ret;
}


void dt_MapData::Put( const RECT* p_rc_view, int off_x, int off_y, bool b_visible_zero_parts )
{
	RECT rc;
	long parts, x, y;
	long x_start, x_end;
	long y_start, y_end;


	x_start = off_x / FIELDGRID; x_end = x_start + ( p_rc_view->right  - p_rc_view->left ) / FIELDGRID + 2;
	y_start = off_y / FIELDGRID; y_end = y_start + ( p_rc_view->bottom - p_rc_view->top  ) / FIELDGRID + 2;
	if( x_end > _w - 1 ) x_end = _w - 1;
	if( y_end > _l - 1 ) y_end = _l - 1;

	for( y = y_start; y <= y_end; y++ )
	{
		for( x = x_start; x <= x_end; x++ )
		{
			parts = _p[ y * _max_w + x ];
			if( b_visible_zero_parts || parts )
			{
				rc.left   = ( parts % MAPPARTS_NUM_H ) * FIELDGRID;
				rc.right  = rc.left + FIELDGRID;
				rc.top    = ( parts / MAPPARTS_NUM_H ) * FIELDGRID;
				rc.bottom = rc.top  + FIELDGRID;
				DxDraw_Put_Clip( p_rc_view, x * FIELDGRID - off_x, y * FIELDGRID - off_y, &rc, _surf_mparts );
			}
		}
	}
}

bool dt_MapData::GetClip( dt_MapData *dst, int src_x, int src_y )
{
	if( src_x < 0 || src_y < 0 ) return false;
	if( src_x + dst->_w > _w   ) return false;
	if( src_y + dst->_l > _l   ) return false;

	const unsigned char *p_src = &_p[ src_x + src_y * _max_w ];
	unsigned       char *p_dst =  dst->_p;

	for( int y = 0; y < dst->_l; y++ )
	{
		for( int x = 0; x < dst->_w; x++ ) *p_dst++ = *p_src++;
		p_src += _max_w -       dst->_w;
	}
	return true;
}

bool dt_MapData::CopyTo( dt_MapData *dst )
{
	if( _w != dst->_w || _l != dst->_l ) return false;

	const unsigned char *p_src = _p;
	unsigned       char *p_dst = dst->_p;

	memcpy( dst->_p, _p, _w * _l );

	dst->location = location;
	return true;
}

void dt_MapData::Fill( unsigned char uc )
{
	memset( _p, uc, _w * _l );
}

void dt_MapData::Location_Wipeout( char shift_x, char shift_y )
{
	location.status = MDLOAD_TryRequest;
	location.x_    += shift_x;
	location.y_    += shift_y;
	memset( _p, 0, _w * _l )   ;
}
