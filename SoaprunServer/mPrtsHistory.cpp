#include <string.h>    // strcmp() / strerror() / strcpy() / memset() / strlen()

#include "mPrtsHistory.h"

mPrtsHistory::mPrtsHistory()
{
	_num = 0;
}

void mPrtsHistory::Push( short x, short y, unsigned char parts )
{
	if( _num >= MAPPARTSHISTORY_NUM )
	{
		_num = MAPPARTSHISTORY_NUM - 1;
		for( int i = 0; i < _num; i++ ) _list[ i ] = _list[ i + 1 ];
	}
	_list[ _num ].x     = x    ;
	_list[ _num ].y     = y    ;
	_list[ _num ].parts = parts;
	_num++;
}

int  mPrtsHistory::Pop( MAPPARTSCHANGE *list )
{
	int num = _num;
	memcpy( list, _list, sizeof(MAPPARTSCHANGE) * _num );
	_num = 0;
	return num;
}

int  mPrtsHistory::Num ()
{
	return _num;
}

void mPrtsHistory::Clear()
{
	_num = 0;
}
