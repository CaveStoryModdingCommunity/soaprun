#include <string.h>    // strcmp() / strerror() / strcpy() / memset() / strlen()

#include "svPosHistory.h"

svPosHistory::svPosHistory()
{
	_num = 0;
	memset( _list, 0, sizeof(POSSHORT) * POSHISTORY_NUM );
}

void svPosHistory::Push( short map_x, short map_y, unsigned int update_id )
{
	if( _num < POSHISTORY_NUM )
	{
		_list[ _num ].x         = map_x    ;
		_list[ _num ].y         = map_y    ;
		_list[ _num ].update_id = update_id;
		_num++;
	}
	else
	{
		for( int i = 0; i < POSHISTORY_NUM - 1; i++ ) _list[ i ] = _list[ i + 1 ];
		_list[ POSHISTORY_NUM - 1 ].x         = map_x    ;
		_list[ POSHISTORY_NUM - 1 ].y         = map_y    ;
		_list[ POSHISTORY_NUM - 1 ].update_id = update_id;
		_num = POSHISTORY_NUM;
	}
}

void svPosHistory::Push( const POSSHORT *p_pos, unsigned int update_id )
{
	if( _num < POSHISTORY_NUM )
	{
		_list[ _num ].x         = p_pos->x ;
		_list[ _num ].y         = p_pos->y ;
		_list[ _num ].update_id = update_id;
		_num++;
	}
	else
	{
		for( int i = 0; i < POSHISTORY_NUM - 1; i++ ) _list[ i ] = _list[ i + 1 ];
		_list[ POSHISTORY_NUM - 1 ].x         = p_pos->x ;
		_list[ POSHISTORY_NUM - 1 ].y         = p_pos->y ;
		_list[ POSHISTORY_NUM - 1 ].update_id = update_id;
		_num = POSHISTORY_NUM;
	}
}

bool svPosHistory::GetLast( short *px, short *py ) const
{
	if( _num <= 0 ) return false;
	*px = _list[ _num - 1 ].x;
	*py = _list[ _num - 1 ].y;
	return true;
}

short svPosHistory::LastX() const { if( _num <= 0 ) return 0; return _list[ _num - 1 ].x; }
short svPosHistory::LastY() const { if( _num <= 0 ) return 0; return _list[ _num - 1 ].y; }

void svPosHistory::ShiftPush( char dir_x, char dir_y, unsigned int update_id )
{
	short x = 0, y = 0;
	GetLast( &x, &y );

	Push( x + dir_x, y + dir_y, update_id );
}

void svPosHistory::Reset( short x, short y, unsigned int update_id )
{
	_num = 0;
	Push( x, y, update_id );
}

int svPosHistory::Get( POSSHORT *list, unsigned int update_id )
{
	if( _num <= 0 ) return 0;

	int num = 0;

	for( int h = 0; h < _num - 1; h++ )
	{
		if( _list[ h ].update_id > update_id )
		{
			list[ num ].x = _list[ h ].x;
			list[ num ].y = _list[ h ].y;
			num++;
		}
	}
	// 必ず１つは送る。
	list[ num ].x = _list[ _num - 1 ].x;
	list[ num ].y = _list[ _num - 1 ].y;
	num++;

	return _num;
}

int svPosHistory::Num( unsigned int update_id )
{
	if( _num <= 0 ) return 0;

	int num = 0;
	for( int h = 0; h < _num - 1; h++ )
	{
		if( _list[ h ].update_id > update_id ) num++;
	}
	// 必ず１つは送る。
	num++;
	return num;
}
