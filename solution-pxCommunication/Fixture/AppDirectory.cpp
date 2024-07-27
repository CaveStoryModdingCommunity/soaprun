#include "StdAfx.h"

bool AppDirectory_Ready( char** pp_path, const char *name )
{
	char path[ MAX_PATH ];

	GetModuleFileName( NULL, path, MAX_PATH );
	PathRemoveFileSpec(      path );
	strcat(                  path, "\\" );
	strcat(                  path, name );
	CreateDirectory(         path, NULL );

	if( !(  *pp_path = (char*)malloc( strlen( path ) + 1 ) ) ) return false;
	memcpy( *pp_path, path,           strlen( path ) + 1 );

	return true;
}

bool AppDirectory_Check( char** pp_path, const char *name )
{
	char path[ MAX_PATH ];

	GetModuleFileName( NULL, path, MAX_PATH );
	PathRemoveFileSpec(      path );
	strcat(                  path, "\\" );
	strcat(                  path, name );
	if( !PathIsDirectory(    path ) ) return false;

	if( !(  *pp_path = (char*)malloc( strlen( path ) + 1 ) ) ) return false;
	memcpy( *pp_path, path,           strlen( path ) + 1 );

	return true;
}

void AppDirectory_Release( char** pp_path )
{
	if( *pp_path ) free( *pp_path );
}