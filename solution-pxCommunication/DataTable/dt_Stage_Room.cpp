#include <StdAfx.h>

#include "dt_Stage.h"

/*

<room>

	<map>
		"NAME"
		unsigned short size;
		data
	</map>

	<map>
		"MAP "
		unsigned short size;
		data
	</map>

	<unit>
		"UNIT"
		unsigned short size;
		data
	</unit>

</room>

*/
/*
#define _CODE_SIZE 4

static const char* _name_code = "NAME";
static const char* _map_code  = "MAP ";
static const char* _unit_code = "UNIT";

const char* dt_Stage::Room_Ext()
{
	return _room_ext ;
}

bool dt_Stage::Room_IsChanged()
{
	if( b_unit_changed || b_map_changed || b_room_changed ) return true;
	return false;
}

bool dt_Stage::Room_New( int w, int l )
{
	FILE *fp;
	char path[ MAX_PATH ];

	int no;

	for( no = 0; no < MAX_STAGEROOM; no++ )
	{
		sprintf( path, "%s\\%s.%s\\%d.%s", _dir_data, _stage_name, _stage_ext, no, _room_ext );
		if( fp = fopen( path, "rb" ) ) fclose( fp );
		else                           break;
	}
	if( no == MAX_STAGEROOM ) return false;

	// room ---
	room_no = no;
	memset( _room_name, 0, BUFFERSIZE_ROOMNAME );
	b_room_changed = true;

	// map ---
	b_map_changed  = true;
	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		_map_tables[ m ]->Clear();
	}
	for( int m = 0; m < MAPLAYERNUM; m++ )
	{
		if( !_map_tables[ m ]->SetSize( w, l, 0 ) ) return false;
	}

	// unit ---
	b_unit_changed = true;
	_unit_main->Clear();

	return true;
}

bool dt_Stage::Room_SaveNow()
{
	bool b_ret = false;
	FILE *fp   = NULL;

	unsigned short size;
	int            sk1, sk2;

	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s.%s\\%d.%s", _dir_data, _stage_name, _stage_ext, room_no, _room_ext );

	if( !( fp = fopen( path, "wb" ) ) ) goto End;

	// name ==== ====
	fwrite( _name_code, sizeof(char), _CODE_SIZE, fp );
	size = strlen( _room_name );
	fwrite( &size     , sizeof(unsigned short),    1, fp );
	fwrite( _room_name, sizeof(char          ), size, fp );

	// map ==== ====
	fwrite( _map_code, sizeof(char), _CODE_SIZE, fp );
	size = 0; fwrite( &size, sizeof(unsigned short), 1, fp ); sk1  = ftell( fp );
	{
		for( int m = 0; m < MAPLAYERNUM; m++ ){ if( !_map_tables[ m ]->Write( fp ) ) goto End; }
	}
	sk2  = ftell( fp ); size = sk2 - sk1;
	fseek ( fp, sk1 - sizeof(unsigned short), SEEK_SET );
	fwrite( &size,    sizeof(unsigned short), 1, fp    );
	fseek( fp, sk2, SEEK_SET );


	// unit ==== ====
	fwrite( _unit_code, sizeof(char), _CODE_SIZE, fp );
	size = 0; fwrite( &size, sizeof(unsigned short), 1, fp ); sk1  = ftell( fp );
	{
		if( !_unit_main->Write( fp ) ) goto End;
	}
	sk2  = ftell( fp ); size = sk2 - sk1;
	fseek ( fp, sk1 - sizeof(unsigned short), SEEK_SET );
	fwrite( &size,    sizeof(unsigned short), 1, fp    );
	fseek( fp, sk2, SEEK_SET );

	b_room_changed = false;
	b_unit_changed = false;
	b_map_changed  = false;

	b_ret = true;
End:
	
	if( fp ) fclose( fp );

	return b_ret;
}


bool dt_Stage::Room_Load( int no )
{
	bool             b_ret = false;
	DDV read;
	char             path[ MAX_PATH ];

	char             code[ 10 ];
	unsigned short   size      ;

	for( int m = 0; m < MAPLAYERNUM; m++ ) _map_tables[ m ]->Clear();
	_unit_main->Clear();

	sprintf( path, "%s\\%s.%s\\%d.%s", _dir_data, _stage_name, _stage_ext, no, _room_ext );
	if( !ddv_Open( NULL, path, NULL, &read ) ) goto End;

	// name ==== ====
	ddv_Read( &code     , sizeof(char          ), _CODE_SIZE, &read );
	ddv_Read( &size     , sizeof(unsigned short),          1, &read );
	if( size >= BUFFERSIZE_ROOMNAME ) size = BUFFERSIZE_ROOMNAME - 1;
	ddv_Read( _room_name, sizeof(char          ), size      , &read );
	_room_name[ size ] = '\0';
	b_room_changed = false;

	// map ==== ====
	ddv_Read( &code, sizeof(char          ), _CODE_SIZE, &read );
	ddv_Read( &size, sizeof(unsigned short),          1, &read );
	for( int m = 0; m < MAPLAYERNUM; m++ ){ if( !_map_tables[ m ]->Read( &read ) ) goto End; }
	b_map_changed    = false ;

	// unit ==== ====
	ddv_Read( &code, sizeof(char          ), _CODE_SIZE, &read );
	ddv_Read( &size, sizeof(unsigned short),          1, &read );
	if( !_unit_main->Read( &read ) )goto End;
	b_unit_changed = false;

	room_no = no;

	b_ret = true  ;
End:

	ddv_Close( &read );

	return b_ret;
}


bool dt_Stage::Room_ReadName( int no, char **pp_name )
{
	bool             b_ret = false;
	DDV read;
	char             path[ MAX_PATH ];
	char             name[ MAX_PATH ];

	char             code[ 10 ];
	unsigned short   size      ;

	sprintf( path, "%s\\%s.%s\\%d.%s", _dir_data, _stage_name, _stage_ext, no, _room_ext );
	if( !ddv_Open( NULL, path, NULL, &read ) ) goto End;

	// name ==== ====
	ddv_Read( &code, sizeof(char          ), _CODE_SIZE, &read );
	ddv_Read( &size, sizeof(unsigned short),          1, &read );
	if( size >= BUFFERSIZE_ROOMNAME ) size = BUFFERSIZE_ROOMNAME - 1;
	ddv_Read( name , sizeof(char          ), size      , &read );
	name[ size ] = '\0';

	if( !size ) goto End;

	*pp_name = (char*)malloc( size + 1 );
	memcpy( *pp_name, name, size + 1 );

	b_ret = true  ;
End:

	ddv_Close( &read );

	return b_ret;
}

const char *dt_Stage::Room_GetName(){ return _room_name; }
bool        dt_Stage::Room_SetName( const char* name )
{
	if( strlen( name ) >= BUFFERSIZE_ROOMNAME ) return false;
	strcpy( _room_name, name );
	b_room_changed = true;
	return true;
}



void dt_Stage::Room_PathForFind( char* path )
{
	sprintf( path, "%s\\%s.%s\\*.%s", _dir_data, _stage_name, _stage_ext, _room_ext );
}

*/
