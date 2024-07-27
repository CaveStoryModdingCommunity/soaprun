#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "dt_Stage.h"

dt_Stage::dt_Stage( const char* dir_data, int max_w, int max_l, const STAGESURFACES* p_surfs )
{
	memset( _stage_name, 0, BUFFERSIZE_STAGENAME );
//	memset( _room_name , 0, BUFFERSIZE_ROOMNAME  );

	_surfs         = *p_surfs;

//	room_no        =     0;
//	b_room_changed = false;

	_stage_ext        = "pxstg"           ;
	_mparts_bmp       = "mparts.bmp"      ;
	_attr_name        = "mparts.pxattr"   ;
//	_room_ext         = "pxroom"          ;
	_map_fname        = "map.bin"         ;
	_unit_fname       = "unit.bin"        ;
	_unit_lcast_bmp   = "unit-lcast.bmp"  ;
	_unit_lsymbol_bmp = "unit-lsymbol.bmp";
	_dir_data         = dir_data          ;

	// Map --------
	b_map_changed     = false;
	map_max_w         = max_w;
	map_max_l         = max_l;

	DxDraw_SubSurface_ReadyBlank( MAPPARTS_BMP_W, MAPPARTS_BMP_H, _surfs.mparts      , FALSE );
//	DxDraw_SubSurface_ReadyBlank( DOOR_BMP_W    , DOOR_BMP_H    , _surfs.door        , FALSE );
	DxDraw_SubSurface_ReadyBlank( FLDOBJ_BMP_W  , FLDOBJ_BMP_L  , _surfs.unit_lcast  , FALSE );
	DxDraw_SubSurface_ReadyBlank( FLDOBJ_BMP_W  , FLDOBJ_BMP_L  , _surfs.unit_lsymbol, FALSE );

	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		_map_copies[ m ] = new dt_MapData( max_w, max_l, _surfs.mparts );
		_map_tables[ m ] = new dt_MapData( max_w, max_l, _surfs.mparts );
	}

	// Attribute --------
	b_attr_changed = false;
	DxDraw_SubSurface_Load( "BMP_ATTRIBUTE1", _surfs.attr1, FALSE, FALSE );
	DxDraw_SubSurface_Load( "BMP_ATTRIBUTE2", _surfs.attr2, FALSE, FALSE );
	for( int i = 0; i < ATTRIBUTELAYERNUM; i++ )
	{
		_atts[ i ] = new dt_MapData( MAX_ATTRIBUTE_HNUM, MAX_ATTRIBUTE_VNUM, i ? _surfs.attr2 : _surfs.attr1 );
		_atts[ i ]->SetSize(         MAX_ATTRIBUTE_HNUM, MAX_ATTRIBUTE_VNUM, 0 );
	}

	// Unit --------
	b_unit_changed = false;
	_unit_main     = new dt_FldObj( MAXFIELDUNIT );
	_unit_copy     = new dt_FldObj( MAXFIELDUNIT );

	DT_FLDOBJ unit;
	memset( &unit, 0, sizeof(DT_FLDOBJ) );
	_unit_copy->Add( &unit );
}

dt_Stage::~dt_Stage()
{
	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		delete _map_copies[ m ];
		delete _map_tables[ m ];
	}
//	DxDraw_SubSurface_Release( _surfs.door         );
	DxDraw_SubSurface_Release( _surfs.mparts       );
	DxDraw_SubSurface_Release( _surfs.unit_lcast   );
	DxDraw_SubSurface_Release( _surfs.unit_lsymbol );

	for( int i = 0; i < ATTRIBUTELAYERNUM; i++ ) delete _atts[ i ];
	DxDraw_SubSurface_Release( _surfs.attr1 );
	DxDraw_SubSurface_Release( _surfs.attr2 );
}

bool dt_Stage::edit_IsChanged()
{
	if( b_map_changed  ||
		b_unit_changed ||
		b_attr_changed ) return true;
	return false;
}

bool dt_Stage::Stage_Save()
{
	if( !_Attribute_Save() ) return false;
//	if( ! Room_SaveNow  () ) return false;
	if( ! Map_SaveNow   () ) return false;
	if( ! Unit_SaveNow  () ) return false;
	b_map_changed  = false;
	b_unit_changed = false;
	b_attr_changed = false;

	return true;
}

int dt_Stage::Stage_ReloadImages()
{
	char path[ MAX_PATH ];
	int  err_flags = 0;
//	DxDraw_PaintRect_Surface( NULL, 0, _surfs.mparts       ); sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _mparts_bmp       ); if( !DxDraw_SubSurface_Reload( path, _surfs.mparts      , TRUE ) ) err_flags |= ERRFLAG_MAPPARTSIMAGE;
	DxDraw_PaintRect_Surface( NULL, 0, _surfs.mparts       ); sprintf( path, "%s\\%s"       , _dir_data,                          _mparts_bmp       ); if( !DxDraw_SubSurface_Reload( path, _surfs.mparts      , TRUE ) ) err_flags |= ERRFLAG_MAPPARTSIMAGE;
//	DxDraw_PaintRect_Surface( NULL, 0, _surfs.door         ); sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _door_bmp         ); if( !DxDraw_SubSurface_Reload( path, _surfs.door        , TRUE ) ) err_flags |= ERRFLAG_DOORIMAGE    ;
	DxDraw_PaintRect_Surface( NULL, 0, _surfs.unit_lcast   ); sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _unit_lcast_bmp   ); if( !DxDraw_SubSurface_Reload( path, _surfs.unit_lcast  , TRUE ) ) err_flags |= ERRFLAG_lCASTIMAGE   ;
	DxDraw_PaintRect_Surface( NULL, 0, _surfs.unit_lsymbol ); sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _unit_lsymbol_bmp ); if( !DxDraw_SubSurface_Reload( path, _surfs.unit_lsymbol, TRUE ) ) err_flags |= ERRFLAG_lSYMBOLIMAGE ;
	return err_flags;
}

int dt_Stage::Stage_Load( const char* stage_name/*, int room_no */ )
{
	bool b_ret = false;
	int  err_flags = 0;

	strcpy( _stage_name, stage_name );
//	dt_Stage::room_no = room_no;

	b_map_changed  = false;
	b_unit_changed = false;
	b_attr_changed = false;

	err_flags |= Stage_ReloadImages();

	// Attribute
	if( !_Attribute_Load()          ) err_flags |= ERRFLAG_ATTRIBUTE;
//	if( !Room_Load      ( room_no ) ) err_flags |= ERRFLAG_ROOM     ;
	if( !Map_Load       ()          ) err_flags |= ERRFLAG_MAP      ;
	if( !Unit_Load      ()          ) err_flags |= ERRFLAG_UNIT     ;

	return err_flags;
}


bool dt_Stage::Stage_New( const char* stage_name )
{
	bool b_ret = false;
	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s.%s", _dir_data, stage_name, _stage_ext );
	if(  PathIsDirectory( path       ) ) goto End;
	if( !CreateDirectory( path, NULL ) ) goto End;

	// 共通のデータフォルダに stage_name ディレクトリを生成する
	strcpy( _stage_name, stage_name );
//	memset( _room_name, 0, BUFFERSIZE_ROOMNAME  );

//	room_no        =     0;
	b_map_changed  = false;
	b_unit_changed = false;
	b_attr_changed = false;
//	b_room_changed = false;

	b_ret = true;
End:
	return b_ret;
}

const char* dt_Stage::Stage_GetName(){ return _stage_name; }

void dt_Stage::edit_GetStageDirectory( char* path_dir )
{
	sprintf( path_dir, "%s\\%s.%s", _dir_data, _stage_name, _stage_ext );
}
