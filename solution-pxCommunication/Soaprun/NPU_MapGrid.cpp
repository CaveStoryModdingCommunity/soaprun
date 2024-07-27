#include <StdAfx.h>
//#include <stdlib.h>     // mode_t / exit() / malloc()
//#include <string.h>     // strcmp() / strerror() / strcpy() / memset() / strlen()

#include "NPU_MapGrid.h"
//#include "DebugLog.h"

NPU_MapGrid:: NPU_MapGrid( int map_w, int map_l )
{
	_map_w = map_w;
	_map_l = map_l;
	_buf_size = ( map_w * map_l + 7 ) / 8;
	if( _p_buf = (unsigned char*)malloc( _buf_size ) )
	{
		memset( _p_buf, 0, _buf_size );
	}
}

NPU_MapGrid::~NPU_MapGrid()
{
//	dlog( "npu mapgrid" );
	if( _p_buf ) free( _p_buf );
	_p_buf = NULL;
}

void NPU_MapGrid::Clear  ()
{
	memset( _p_buf, 0, _buf_size );
}

void NPU_MapGrid::On     ( int x, int y )
{
	if( x < 0 || x >= _map_w || y < 0 || y >= _map_w ) return;

	int no = x + y * _map_w;
	_p_buf[ no/8 ] |=  ( 0x01 << (no%8) );
}

void NPU_MapGrid::Off    ( int x, int y )
{
	if( x < 0 || x >= _map_w || y < 0 || y >= _map_w ) return;

	int no = x + y * _map_w;
	_p_buf[ no/8 ] &= ~( 0x01 << (no%8) );
}

bool NPU_MapGrid::IsOn   ( int x, int y )
{
	if( x < 0 || x >= _map_w || y < 0 || y >= _map_w ) return false;

	int no = x + y * _map_w;
	if( _p_buf[ no/8 ] & ( 0x01 << (no%8) ) ) return true;
	return false;
}

