#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "dt_Stage.h"

//#define _CODE_SIZE 4

//static const char* _map_code  = "MAP ";

bool dt_Stage::Map_SetSize( int w, int l, int align_flags )
{
	b_map_changed = true;
	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		if( !_map_tables[ m ]->SetSize( w, l, align_flags ) ) return false;
	}
	return true;
}
void dt_Stage::Map_GetSize( int *pw, int *pl )
{
	*pw = _map_tables[ 0 ]->get_w();
	*pl = _map_tables[ 0 ]->get_l();
}

void dt_Stage::Map_Put( int layer, const RECT* p_rc_view, int off_x, int off_y, bool b_visible_zero_parts )
{
	_map_tables[ layer ]->Put( p_rc_view, off_x, off_y, b_visible_zero_parts );
}

void dt_Stage::_Map_PutAttribute( const RECT *p_rc_view, int x, int y, int parts )
{
	RECT rc;
	int att_x =parts % MAPPARTS_NUM_H;
	int att_y =parts / MAPPARTS_NUM_H ;

	int att_index;

	for( int a = 0; a < 2; a++ )
	{
		att_index = _atts[ a ]->GetParts( parts % MAPPARTS_NUM_H, parts / MAPPARTS_NUM_H );

		rc.left   = ( att_index % ATTRIBUTE_NUM_H ) * FIELDGRID;
		rc.right  = rc.left                         + FIELDGRID;
		rc.top    = ( att_index / ATTRIBUTE_NUM_H ) * FIELDGRID;
		rc.bottom = rc.top                          + FIELDGRID;
		DxDraw_Put_Clip( p_rc_view, x, y, &rc, a ? _surfs.attr2 : _surfs.attr1 );
	}
}

void dt_Stage::Map_Put_Attribute( int layer, const RECT* p_rc_view, int off_x, int off_y )
{
	long parts, x, y;
	long x_start, x_end;
	long y_start, y_end;

	int w = _map_tables[ layer ]->get_w();
	int l = _map_tables[ layer ]->get_l();

	x_start = off_x / FIELDGRID; x_end = x_start + ( p_rc_view->right  - p_rc_view->left ) / FIELDGRID + 2;
	y_start = off_y / FIELDGRID; y_end = y_start + ( p_rc_view->bottom - p_rc_view->top  ) / FIELDGRID + 2;
	if( x_end > w - 1 ) x_end = w - 1;
	if( y_end > l - 1 ) y_end = l - 1;

	for( y = y_start; y <= y_end; y++ )
	{
		for( x = x_start; x <= x_end; x++ )
		{
			parts = _map_tables[ layer ]->GetParts( x, y );
			_Map_PutAttribute( p_rc_view, x * FIELDGRID - off_x, y * FIELDGRID - off_y, parts );
		}
	}
}


//
// 以下、編集用コピーペーストメソッド
//

long dt_Stage::Map_copy_GetPartsIndex( int layer )
{
	if( layer >= MAPLAYERNUM ) return 0;
	return _map_copies[ layer ]->GetParts( 0, 0 );
}

void dt_Stage::Map_copy_GetSize( int *pw, int *pl, int layer )
{
	if( layer >= MAPLAYERNUM ) layer = 0;

	*pw = _map_copies[ layer ]->get_w();
	*pl = _map_copies[ layer ]->get_l();
}

void dt_Stage::Map_copy_Copy( int layer, int x1, int y1, int x2, int y2, BOOL bMapParts )
{
	int w, l, x, y;

	if( layer >= MAPLAYERNUM ) return;

	dt_MapData* p_map  = _map_tables[ layer ];
	dt_MapData* p_copy = _map_copies[ layer ];

	int map_w = p_map->get_w();
	int map_l = p_map->get_l();

	if( x1 > x2 ){ w = x1; x1 = x2; x2 = w; }
	if( y1 > y2 ){ w = y1; y1 = y2; y2 = w; }

	if( bMapParts )
	{
		if( x1 < 0 ) x1 = 0; if( x1 > MAPPARTS_NUM_H - 1 ) x1 = MAPPARTS_NUM_H - 1;
		if( y1 < 0 ) y1 = 0; if( y1 > MAPPARTS_NUM_V - 1 ) y1 = MAPPARTS_NUM_V - 1;
		if( x2 < 0 ) x2 = 0; if( x2 > MAPPARTS_NUM_H - 1 ) x2 = MAPPARTS_NUM_H - 1;
		if( y2 < 0 ) y2 = 0; if( y2 > MAPPARTS_NUM_V - 1 ) y2 = MAPPARTS_NUM_V - 1;
	}
	else
	{
		if( x1 < 0 ) x1 = 0; if( x1 > map_w - 1 ) x1 = map_w - 1;
		if( y1 < 0 ) y1 = 0; if( y1 > map_l - 1 ) y1 = map_l - 1;
		if( x2 < 0 ) x2 = 0; if( x2 > map_w - 1 ) x2 = map_w - 1;
		if( y2 < 0 ) y2 = 0; if( y2 > map_l - 1 ) y2 = map_l - 1;
	}

	w = x2 - x1 + 1;
	l = y2 - y1 + 1;

	p_copy->SetSize( w, l, 0 );


	if( bMapParts )
	{
		for( y = 0; y < l; y++ )
		{
			for( x = 0; x < w; x++ ) p_copy->SetParts( x, y, (unsigned char)( ( y1 + y ) * MAPPARTS_NUM_H + x1 + x ) );
		}
	}
	else
	{

		for( y = 0; y < l; y++ )
		{
			for( x = 0; x < w; x++ ) p_copy->SetParts( x, y, p_map->GetParts( x1 + x, y1 + y ) );
		}
	}
}

void dt_Stage::Map_copy_Paste( int layer, int x1, int y1 )
{
	if( layer >= MAPLAYERNUM ) return;

	b_map_changed = true;

	dt_MapData* p_map  = _map_tables[ layer ];
	dt_MapData* p_copy = _map_copies[ layer ];

	int map_w = p_map->get_w();
	int map_l = p_map->get_l();

	int cpy_w = p_copy->get_w();
	int cpy_l = p_copy->get_l();

	if( x1 < 0 ) x1 = 0; if( x1 > map_w - 1 ) x1 = map_w - 1;
	if( y1 < 0 ) y1 = 0; if( y1 > map_l - 1 ) y1 = map_l - 1;

	for( int y = 0; y < cpy_l; y++ )
	{
		for( int x = 0; x < cpy_w; x++ )
		{
			p_map->SetParts( x1 + x, y1 + y, p_copy->GetParts( x, y ) );
		}
	}
}

long dt_Stage::Map_GetAttribute( int att_layer, int map_x, int map_y )
{
	long parts;
	long attr = 0;

	if( map_x < 0 || map_x >= _map_tables[ 0 ]->get_w() ||
		map_y < 0 || map_y >= _map_tables[ 0 ]->get_l() ) return 0xf;

	for( long m = 0; m < MAPLAYERNUM; m++ )
	{
		parts  = _map_tables[ m ]->GetParts( map_x, map_y );
		attr  |= _atts[ att_layer ]->GetParts( parts % MAPPARTS_NUM_H, parts / MAPPARTS_NUM_H );
	}

	return attr;
}

long dt_Stage::Map_GetAttribute_CheckTo( int map_x, int map_y )
{
	return Map_GetAttribute( 0, map_x, map_y ) &
	     (~Map_GetAttribute( 1, map_x, map_y ) );
}

bool dt_Stage::Map_New( int w, int l )
{
	b_map_changed  = true;
	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		_map_tables[ m ]->Clear();
	}
	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		if( !_map_tables[ m ]->SetSize( w, l, 0 ) ) return false;
	}
	return true;
}

bool dt_Stage::Map_SaveNow()
{
	bool b_ret = false;
	FILE *fp   = NULL;

	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _map_fname );

	if( !( fp = fopen( path, "wb" ) ) ) goto End;

	for( int m = 0; m < MAPLAYERNUM; m++ ){ if( !_map_tables[ m ]->Write( fp ) ) goto End; }

	b_map_changed  = false;

	b_ret = true;
End:
	
	if( fp ) fclose( fp );

	return b_ret;
}

bool dt_Stage::Map_Load( void )
{
	bool             b_ret = false;
//	DDV read;
	char             path[ MAX_PATH ];
	FILE *fp = NULL;

	for( int m = 0; m < MAPLAYERNUM; m++ ) _map_tables[ m ]->Clear();

	sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _map_fname );

	if( !( fp = fopen( path, "rb" ) ) ) goto End;

//	if( !ddv_Open( NULL, path, NULL, &read ) ) goto End;

	for( int m = 0; m < MAPLAYERNUM; m++ ){ if( !_map_tables[ m ]->Read( fp ) ) goto End; }

	b_map_changed    = false ;

	b_ret = true  ;
End:

	if( fp ) fclose( fp );

//	ddv_Close( &read );

	return b_ret;
}

bool dt_Stage::Map_GetClip( dt_MapData *dst, int src_x, int src_y )
{
	return _map_tables[ 0 ]->GetClip( dst, src_x, src_y );
}
