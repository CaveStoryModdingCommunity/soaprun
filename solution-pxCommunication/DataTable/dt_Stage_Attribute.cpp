#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "dt_Stage.h"


void dt_Stage::Attribute_Clear( void )
{
	b_attr_changed = true;
	for( int i = 0; i < ATTRIBUTELAYERNUM; i++ ) _atts[ i ]->Clear();
}

bool dt_Stage::Attribute_SetSize( int w, int h, int flags )
{
	b_attr_changed = true;
	for( int i = 0; i < ATTRIBUTELAYERNUM; i++ )
	{
		if( !_atts[ i ]->SetSize( w, h, flags ) ) return false;
	}
	return true;
}


bool dt_Stage::_Attribute_Save()
{
	bool  b_ret = false;
	FILE* fp    = NULL ;
	char  path[ MAX_PATH ];

	sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _attr_name );

	if( !( fp = fopen( path, "wb" ) ) )goto End;
	for( long i = 0; i < ATTRIBUTELAYERNUM; i++ ){ if( !_atts[ i ]->Write( fp ) ) goto End; }

	b_attr_changed = false;

	b_ret = true;
End:
	
	if( fp ) fclose( fp );

	return b_ret;
}

bool dt_Stage::_Attribute_Load()
{
	bool             b_ret = false;
//	DDV read;
	FILE *fp = NULL;
	char             path[ MAX_PATH ];

	sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _attr_name );

	Attribute_Clear();

//	if( !ddv_Open( NULL, path, NULL, &read ) ) goto End;
	if( !( fp = fopen( path, "rb" ) ) ) goto End;

	for( int i = 0; i < ATTRIBUTELAYERNUM; i++ ){ if( !_atts[ i ]->Read( fp ) ) goto End; }

	b_attr_changed = false;

	b_ret = TRUE;
End:

	if( fp ) fclose( fp );

//	ddv_Close( &read );
	if( !b_ret ) Attribute_Clear();

	return b_ret;
}

void dt_Stage::Attribute_GetSize( int *pw, int *pl )
{
	*pw = _atts[ 0 ]->get_w();
	*pl = _atts[ 0 ]->get_l();
}

void dt_Stage::Attribute_copy_Paste( int layer, int x, int y, unsigned char parts_index )
{
	if( layer >= ATTRIBUTELAYERNUM ) return;

	b_attr_changed     = true;

	dt_MapData* p_map  = _atts[ layer ];

	int map_w = p_map->get_w( );
	int map_l = p_map->get_l( );

	if( x < 0 ) x = 0; if( x > map_w - 1 ) x = map_w - 1;
	if( y < 0 ) y = 0; if( y > map_l - 1 ) y = map_l - 1;

	p_map->SetParts( x, y, parts_index );
}

void dt_Stage::Attribute_Put( int layer, RECT* p_rc_view, int off_x, int off_y )
{
	_atts[ layer ]->Put( p_rc_view, off_x, off_y, true );
}

bool dt_Stage::Attribute_Get( dt_MapData *dst )
{
	dst->SetSize( _atts[ 0 ]->get_w(), _atts[ 0 ]->get_l(), 0 );
	return _atts[ 0 ]->GetClip( dst, 0, 0 );
}
