#include <StdAfx.h>

#include "PortNo.h"

const char *_filename = "data-server.bin";


PortNo::PortNo()
{
	PortNo::Load();
}

bool PortNo::Load()
{
	char path[ MAX_PATH ];
	bool b_ret = false;

	sprintf( path, "%s\\%s", g_dir_profile, _filename );

	FILE *fp = NULL;
	char check;

	if( !( fp = fopen( path, "rb" ) ) ) goto End;
	if( fread( &port_no  , sizeof(int), 1, fp ) != 1 ) goto End;
	if( fread( &check    ,           1, 1, fp ) >  0 ) goto End;

	b_ret = true;

End:

	if( fp ) fclose( fp );
	if( !b_ret )
	{
		port_no = 0;
	}
	return b_ret;
}

bool PortNo::Save()
{
	char path[ MAX_PATH ];
	bool b_ret = false;

	sprintf( path, "%s\\%s", g_dir_profile, _filename );

	FILE *fp = NULL;

	if( fp = fopen( path, "wb" ) )
	{
		if( fwrite( &port_no  , sizeof(int), 1, fp ) != 1 ) goto End;
		fclose( fp );
	}
	b_ret = true;

End:
	return b_ret;
}
