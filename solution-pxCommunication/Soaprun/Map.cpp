#include <StdAfx.h>

#include "../Fixture/ShiftJIS.h"
#include "../Fixture/RingBuffer.h"
#include "../Fixture/DebugLog.h"

#include "../Dx/DxDraw.h"

#include "Map.h"
#include "RequestResponse.h"

#include "../DataTable/dt_Stage.h"

#define _MAPPARTS_NUM    ((ROOMGRIDNUM_V+1) * (ROOMGRIDNUM_H+1))

extern RingBuffer *g_ring_req;


void Map::Pane_Update()
{
	int param;
	int w = ROOMGRIDNUM_H + 1;
	int l = ROOMGRIDNUM_V + 1;
	int ani_num;

	for( int y = 0; y < l; y++ )
	{
		for( int x = 0; x < w; x++ )
		{
			param = _nine[ 1 ][ 1 ]->GetParts( x, y );
			if( param == 8 ) ani_num = 2;
			else             ani_num = 1;
			_parts[ y * w + x ]->SetAnimation( param % MPARTSGRIDNUM_H, param / MPARTSGRIDNUM_H, ani_num, 16 );
			_parts[ y * w + x ]->param = param;
		}
	}
}

void Map::_ClearParts( void )
{
	memset( _name , 0, BUFSIZE_MAPNAME );

	for( int y = 0; y < 3; y++ ){ for( int x = 0; x < 3; x++ ) _nine[ y ][ x ]->Fill( 1 ); }

	Pane_Update();
}

Map::Map()
{
	int map_w = ROOMGRIDNUM_H + 1;
	int map_l = ROOMGRIDNUM_V + 1;

	_att  = new dt_MapData( MPARTSATTNUM_H, MPARTSATTNUM_V, 0 );
	_att ->SetSize        ( MPARTSATTNUM_H, MPARTSATTNUM_V, 0 );

	for( int y = 0; y < 3; y++ )
	{
		for( int x = 0; x < 3; x++ )
		{
			_nine[ y ][ x ] = new dt_MapData( map_w, map_l, 0 );
			_nine[ y ][ x ]->SetSize        ( map_w, map_l, 0 );
		}
	}

	_pane  = new Pane( _MAPPARTS_NUM );
	_pane->SetType_View( 0, 0, FIELDVIEW_W, FIELDVIEW_H );

	_parts = (Pane**)malloc( sizeof(Pane*) * _MAPPARTS_NUM );

	for( int i = 0; i < _MAPPARTS_NUM; i++ )
	{
		_parts[ i ] = new Pane();
		int x = ( i % map_w ) * MAPPARTS_W - MAPPARTS_hfW;
		int y = ( i / map_w ) * MAPPARTS_H - MAPPARTS_hfH;
		_parts[ i ]->SetType_Image( x, y, MAPPARTS_W, MAPPARTS_H, SfNo_MapParts );
		_pane->Child_Add( _parts[ i ] );
	}

	_ClearParts();
}

Map::~Map()
{
	if( _pane  ) delete _pane ;
	if( _parts )
	{
		for( int i = 0; i < _MAPPARTS_NUM; i++ )
		{
			if( _parts[ i ] ) delete _parts[ i ];
		}
		free( _parts );
	}
	for( int y = 0; y < 3; y++ ){ for( int x = 0; x < 3; x++ ) if( _nine[ y ][ x ] ) delete _nine[ y ][ x ]; }
}

typedef struct
{
	int x, y;
}
_XYSTRUCT;

static const _XYSTRUCT _xy_table[ 9 ] =
{
	{ 1, 1 }, // ’†‰›
				 
	{ 2, 1 }, //  « 
	{ 0, 1 }, //  ª 
	{ 1, 0 }, //  © 
	{ 1, 2 }, //  ¨ 
				 
	{ 0, 0 }, // ©ª
	{ 0, 2 }, // ¨ª
	{ 2, 0 }, // ©«
	{ 2, 2 }, // ¨«
};

void Map::TryRequest()
{
	dt_MapData    *p ;

	reqROOM req;
	memcpy( req.req_name, g_req_names[ clREQUEST_Room ], BUFFERSIZE_REQNAME );

	for( int i = 0; i < 9; i++ )
	{
		p = _nine[ _xy_table[ i ].y ][ _xy_table[ i ].x ];
		if( p->location.status == MDLOAD_TryRequest )
		{
			req.x = p->location.x_;
			req.y = p->location.y_;
			if( g_ring_req->bin_Push( &req, sizeof(req) ) ) p->location.status = MDLOAD_Requested;
		}
	}
}

void Map::Location_Get( char *p_lct_x, char *p_lct_y ) const
{
	*p_lct_x = _nine[ 1 ][ 1 ]->location.x_;
	*p_lct_y = _nine[ 1 ][ 1 ]->location.y_;
}

void Map::Location_GetOffset( int  *p_off_x  , int  *p_off_y   ) const
{
	*p_off_x = _nine[ 1 ][ 1 ]->location.x_ * ROOMGRIDNUM_H * MAPPARTS_W;
	*p_off_y = _nine[ 1 ][ 1 ]->location.y_ * ROOMGRIDNUM_V * MAPPARTS_H;
}

bool Map::Location_Set( char location_x, char location_y )
{
	bool b_ret = false   ;

	if( location_x < 0 || location_y < 0 ) goto End;

	sprintf( _name, "?" );

	dt_MapData *p ;

	for( char y = 0; y < 3; y++ )
	{
		for( char x = 0; x < 3; x++ )
		{
			p = _nine[ y ][ x ];
			p->location.status = MDLOAD_TryRequest ;
			p->location.x_     = location_x - 1 + x;
			p->location.y_     = location_y - 1 + y;
		}
	}

	TryRequest();

	Pane_Update();

	b_ret = true;
End:

	if( !b_ret ) _ClearParts();
	_pane->SetPos( 0, 0 );

	return b_ret;
}

bool Map::Location_Check( char dir_x, char dir_y )
{
	if( _nine[ 1 + dir_y ][ 1 + dir_x ]->location.status == MDLOAD_Loaded ) return true;
	return false;
}

void Map::_Nine_Wipeout( char x, char y, char dir_x, char dir_y )
{
	dt_MapData *p = _nine[ y ][ x ];

	p->Location_Wipeout( dir_x, dir_y );
}

bool Map::Location_Shift( char shift_x, char shift_y )
{
	if(      shift_x < 0 )
	{
		for( char x = 2; x > 0; x-- )
		{
			for( char y = 0; y < 3; y++ ) _nine[ y ][ x - 1 ]->CopyTo( _nine[ y ][ x ] );
		}
		for( char y = 0; y < 3; y++ ) _Nine_Wipeout( 0, y, shift_x, shift_y );
	}
	else if( shift_x > 0 )
	{
		for( char x = 0; x < 2; x++ )
		{
			for( char y = 0; y < 3; y++ ) _nine[ y ][ x + 1 ]->CopyTo( _nine[ y ][ x ] );
		}
		for( char y = 0; y < 3; y++ ) _Nine_Wipeout( 2, y, shift_x, shift_y );
	}

	if(      shift_y < 0 )
	{
		for( char y = 2; y > 0; y-- )
		{
			for( char x = 0; x < 3; x++ ) _nine[ y - 1 ][ x ]->CopyTo( _nine[ y ][ x ] );
		}
		for( char x = 0; x < 3; x++ ) _Nine_Wipeout( x, 0, shift_x, shift_y );
	}
	else if( shift_y > 0 )
	{
		for( char y = 0; y < 2; y++ )
		{
			for( char x = 0; x < 3; x++ ) _nine[ y + 1 ][ x ]->CopyTo( _nine[ y ][ x ] );
		}
		for( char x = 0; x < 3; x++ ) _Nine_Wipeout( x, 2, shift_x, shift_y );
	}

	Pane_Update();

	return true;
}

bool Map::IsSide( short fld_x, short fld_y, char *p_dir_x, char *p_dir_y ) const
{
	if( p_dir_x ) *p_dir_x = 0;
	if( p_dir_y ) *p_dir_y = 0;

	if     ( fld_x == 0             ){ if( p_dir_x ) *p_dir_x = -1; }
	else if( fld_x == ROOMGRIDNUM_H ){ if( p_dir_x ) *p_dir_x =  1; }
	else if( fld_y == 0             ){ if( p_dir_y ) *p_dir_y = -1; }
	else if( fld_y == ROOMGRIDNUM_V ){ if( p_dir_y ) *p_dir_y =  1; }
	else return false;
	return true;
}

const char *Map::Name    () const{ return _name; }
Pane       *Map::Pane_Get() const{ return _pane; }
/*
bool Map::Attribute_Load()
{
	return g_stage->Attribute_Get( _att );
}
*/

unsigned char Map::Attribute_Get( int map_x, int map_y ) const
{
	unsigned char parts;
	unsigned char att = 0;

	if( map_x < 0 || map_x > ROOMGRIDNUM_H || map_y < 0 || map_y > ROOMGRIDNUM_V ) return 0xf;

	parts  = _nine[ 1 ][ 1 ]->GetParts( map_x, map_y );
	att   |= _att ->GetParts( parts % MPARTSGRIDNUM_H, parts / MPARTSGRIDNUM_H );

	return att;
}

void Map::Attribute_Set( int w, int l, const unsigned char *p )
{
	for( int y = 0; y < l; y++ )
	{
		for( int x = 0; x < w; x++ )
		{
			_att->SetParts( x, y, *p++ );
		}
	}
}

bool Map::Room_Set( char location_x, char location_y, const unsigned char *p )
{
	int w = ROOMGRIDNUM_H + 1;
	int l = ROOMGRIDNUM_V + 1;

	for( int ry = 0; ry < 3; ry++ )
	{
		for( int rx = 0; rx < 3; rx++ )
		{
			dt_MapData *p_room = _nine[ ry ][ rx ];
			if( p_room->location.x_ == location_x &&
				p_room->location.y_ == location_y )
			{
				for( int y = 0; y < l; y++ )
				{
					for( int x = 0; x < w; x++ )
					{
						p_room->SetParts( x, y, *p++ );
					}
				}
				p_room->location.status = MDLOAD_Loaded;
				return true;
			}
		}
	}
	return false;
}

void Map::SetMparts( short fld_x, short fld_y, unsigned char parts )
{
	int lct_x = fld_x / ROOMGRIDNUM_H;
	int lct_y = fld_y / ROOMGRIDNUM_V;

	for( int ry = 0; ry < 3; ry++ )
	{
		for( int rx = 0; rx < 3; rx++ )
		{
			dt_MapData *p_room = _nine[ ry ][ rx ];
			if( p_room->location.x_ == lct_x && p_room->location.y_ == lct_y )
			{
				p_room->SetParts( fld_x%ROOMGRIDNUM_H, fld_y%ROOMGRIDNUM_V, parts );
				Pane_Update();
				return;
			}
		}
	}
}

unsigned char Map::GetMparts( short fld_x, short fld_y ) const
{
	int lct_x = fld_x / ROOMGRIDNUM_H;
	int lct_y = fld_y / ROOMGRIDNUM_V;

	for( int ry = 0; ry < 3; ry++ )
	{
		for( int rx = 0; rx < 3; rx++ )
		{
			dt_MapData *p_room = _nine[ ry ][ rx ];
			if( p_room->location.x_ == lct_x && p_room->location.y_ == lct_y )
			{
				return p_room->GetParts( fld_x%ROOMGRIDNUM_H, fld_y%ROOMGRIDNUM_V );
			}
		}
	}
	return 0;
}
