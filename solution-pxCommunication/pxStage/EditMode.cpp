#include <StdAfx.h>


#include "resource.h"
#include "EditMode.h"

extern char g_dir_profile[];

static enum_EditMode _mode          = enum_EditMode_Map;
static int           _layer         = 0;
static bool          _b_hide_layer  = false;
static const char*   _mode_file = "pxstage.mode";

void EditMode_Initialize( HMENU hMenu )
{
	FILE* fp;
	char  path[ MAX_PATH ];
	int           layer =                 0;
	enum_EditMode mode  = enum_EditMode_Map;

	sprintf( path, "%s\\%s", g_dir_profile, _mode_file );

	if( fp = fopen( path, "rb" ) )
	{
		fread( &mode , sizeof(enum_EditMode), 1, fp );
		fread( &layer, sizeof(int          ), 1, fp );
		fclose( fp );
	}
	EditMode_Set( hMenu, mode, layer );
}

void EditMode_Release( void )
{
	FILE* fp;
	char  path[ MAX_PATH ];

	sprintf( path, "%s\\%s", g_dir_profile, _mode_file );

	if( fp = fopen( path, "wb" ) )
	{
		fwrite( &_mode , sizeof(enum_EditMode), 1, fp );
		fwrite( &_layer, sizeof(int          ), 1, fp );
		fclose( fp );
	}
}

void EditMode_Set( HMENU hMenu, enum_EditMode mode, int layer )
{
	_mode  = mode ;
	_layer = layer;
	CheckMenuItem( hMenu, IDM_MODE_MAP      , MF_BYCOMMAND|MF_UNCHECKED );
//	CheckMenuItem( hMenu, IDM_MODE_FIELDDOOR, MF_BYCOMMAND|MF_UNCHECKED );
	CheckMenuItem( hMenu, IDM_MODE_FIELDUNIT, MF_BYCOMMAND|MF_UNCHECKED );
	CheckMenuItem( hMenu, IDM_MODE_ATT1     , MF_BYCOMMAND|MF_UNCHECKED );
	CheckMenuItem( hMenu, IDM_MODE_ATT2     , MF_BYCOMMAND|MF_UNCHECKED );

	UINT idm_table_att[] =
	{
		IDM_MODE_ATT1,
		IDM_MODE_ATT2,
	};

	switch( _mode )
	{
	case enum_EditMode_Map      : CheckMenuItem( hMenu, IDM_MODE_MAP          , MF_BYCOMMAND|MF_CHECKED ); break;
//	case enum_EditMode_FieldDoor: CheckMenuItem( hMenu, IDM_MODE_FIELDDOOR    , MF_BYCOMMAND|MF_CHECKED ); break;
	case enum_EditMode_FldObj   : CheckMenuItem( hMenu, IDM_MODE_FIELDUNIT    , MF_BYCOMMAND|MF_CHECKED ); break;
	case enum_EditMode_Attribute: CheckMenuItem( hMenu, idm_table_att[ layer ], MF_BYCOMMAND|MF_CHECKED ); break;
	case enum_EditMode_Resident : CheckMenuItem( hMenu, IDM_MODE_RESIDENT     , MF_BYCOMMAND|MF_CHECKED ); break;
	}
}

enum_EditMode EditMode_Get( void )
{
	return _mode;
}

int EditMode_Layer_Get( void )
{
	return _layer;
}

