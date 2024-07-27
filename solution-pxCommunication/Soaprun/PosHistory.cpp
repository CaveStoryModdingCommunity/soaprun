#include <StdAfx.h>

#include "PosHistory.h"

PosHistory::PosHistory()
{
	_num = 0;
	memset( _list, 0, sizeof(POSSHORT) * POSHISTORY_NUM );
}

void PosHistory::Reset( const POSSHORT *p_pos )
{
	memset( _list, 0, sizeof(POSSHORT) * POSHISTORY_NUM );
	_list[ 0 ] = *p_pos;
	_num = 1;
}
void PosHistory::Reset( short fld_x, short fld_y )
{
	memset( _list, 0, sizeof(POSSHORT) * POSHISTORY_NUM );
	_list[ 0 ].x = fld_x;
	_list[ 0 ].y = fld_y;
	_num = 1;
}

void PosHistory::PushPos( short fld_x, short fld_y )
{
	bool b_override = false;

	if( _num > 1 )
	{
		if(      fld_x == _list[ _num - 1 ].x && fld_x == _list[ _num - 2 ].x ) b_override = true;
		else if( fld_y == _list[ _num - 1 ].y && fld_y == _list[ _num - 2 ].y ) b_override = true;
	}

	if     ( b_override            ){ _list[ _num - 1 ].x = fld_x; _list[ _num - 1 ].y = fld_y;         }
	else if( _num < POSHISTORY_NUM ){ _list[ _num     ].x = fld_x; _list[ _num     ].y = fld_y; _num++; }
	else
	{
		_num = POSHISTORY_NUM;
		for( int i = 0; i < POSHISTORY_NUM - 1; i++ ) _list[ i ] = _list[ i + 1 ];
		_list[ _num - 1 ].x = fld_x;
		_list[ _num - 1 ].y = fld_y;
	}
}

void PosHistory::PushPos( const POSSHORT *p_pos )
{
	bool b_override = false;

	if( _num > 1 )
	{
		if(      p_pos->x == _list[ _num - 1 ].x && p_pos->x == _list[ _num - 2 ].x ) b_override = true;
		else if( p_pos->y == _list[ _num - 1 ].y && p_pos->y == _list[ _num - 2 ].y ) b_override = true;
	}
	if     ( b_override            ){ _list[ _num - 1 ] = *p_pos;         }
	else if( _num < POSHISTORY_NUM ){ _list[ _num     ] = *p_pos; _num++; }
	else
	{
		_num = POSHISTORY_NUM;
		for( int i = 0; i < POSHISTORY_NUM - 1; i++ ) _list[ i ] = _list[ i + 1 ];
		_list[ _num - 1 ] = *p_pos;
	}
}

void PosHistory::ShiftPush( char dir_x, char dir_y )
{
	PushPos( LastX() + dir_x, LastY() + dir_y );
}

short PosHistory::LastX     () const{ if( _num <= 0 ) return 0; return _list[ _num - 1 ].x;                   }
short PosHistory::LastY     () const{ if( _num <= 0 ) return 0; return _list[ _num - 1 ].y;                   }
int   PosHistory::LastX_Real() const{ if( _num <= 0 ) return 0; return _list[ _num - 1 ].x * VS * MAPPARTS_W; }
int   PosHistory::LastY_Real() const{ if( _num <= 0 ) return 0; return _list[ _num - 1 ].y * VS * MAPPARTS_H; }
short PosHistory::NextX     () const{ if( _num <= 0 ) return 0; return _list[        0 ].x;                   }
short PosHistory::NextY     () const{ if( _num <= 0 ) return 0; return _list[        0 ].y;                   }
int   PosHistory::NextX_Real() const{ if( _num <= 0 ) return 0; return _list[        0 ].x * VS * MAPPARTS_W; }
int   PosHistory::NextY_Real() const{ if( _num <= 0 ) return 0; return _list[        0 ].y * VS * MAPPARTS_H; }

int PosHistory::PopOne()
{
	if( _num > 1 )
	{
		for( int h = 0; h < _num - 1; h++ ) _list[ h ] = _list[ h + 1 ];
		_num--;
	}
	return _num;
}

int PosHistory::Pop( POSSHORT *list )
{
	int num = _num;
	if( _num > 0 )
	{
		memcpy( list, _list, sizeof(POSSHORT) * _num );
		_list[ 0 ] = _list[ _num - 1 ];
		_num = 1;
	}
	else num = 0;
	return num;
}

