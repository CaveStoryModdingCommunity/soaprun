#include <stdlib.h>     // mode_t / exit() / malloc()
#include <string.h>     // strcmp() / strerror() / strcpy() / memset() / strlen()

#include "SharedDefines.h"

#include "dt_MapData.h"
#include "NPU_MapGrid.h"
#include "DebugLog.h"


NPU_MapGrid:: NPU_MapGrid( const dt_MapData *map, const dt_MapData *att )
{
	_map = map;
	_att = att;

	_buf_size = ( map->get_w() * map->get_l() + 7 ) / 8;
	if( _p_buf = (unsigned char*)malloc( _buf_size ) )
	{
		memset( _p_buf, 0, _buf_size );
	}

	int map_w = _map->get_w();
	int map_l = _map->get_l();

	for( int y = 0; y < map_l; y++ )
	{
		for( int x = 0; x < map_w; x++ )
		{
			unsigned char parts1 = _map->GetParts( x, y );
			unsigned char parts2 = _att->GetParts( parts1&0xf, parts1/0x10 );
			if( parts2 == mPartAttr_Block || parts2 == mPartAttr_bNPU ) On( x, y );
		}
	}

	dlog( "NPU map grid( w:%d l:%d size:%d )", map_w, map_l, _buf_size );

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
	int no = x + y * _map->get_w();
	_p_buf[ no/8 ] |=  ( 0x01 << (no%8) );
}


void NPU_MapGrid::Off    ( int x, int y )
{
	int no = x + y * _map->get_w();
	_p_buf[ no/8 ] &= ~( 0x01 << (no%8) );
	Update( x, y );
}

void NPU_MapGrid::Update ( int x, int y )
{
	unsigned char parts1 = _map->GetParts( x, y );
	unsigned char parts2 = _att->GetParts( parts1&0xf, parts1/0x10 );
	int no = x + y * _map->get_w();

	if( parts2 == mPartAttr_Block || parts2 == mPartAttr_bNPU )
	{
		_p_buf[ no/8 ] |=  ( 0x01 << (no%8) );
	}
	else
	{
		_p_buf[ no/8 ] &= ~( 0x01 << (no%8) );
	}
}

bool NPU_MapGrid::IsOn   ( int x, int y )
{
	int no = x + y * _map->get_w();
	if( _p_buf[ no/8 ] & ( 0x01 << (no%8) ) ) return true;
	return false;
}

