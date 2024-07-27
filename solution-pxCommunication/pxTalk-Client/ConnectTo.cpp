#include <StdAfx.h>

#include "ConnectTo.h"


const char *_filename = "data-client.bin";

ConnectTo::ConnectTo()
{
	ConnectTo::Load();
}

bool ConnectTo::Load()
{
	char path[ MAX_PATH ];
	bool b_ret = false;

	sprintf( path, "%s\\%s", g_dir_profile, _filename );

	FILE *fp = NULL;
	char check;

	if( !( fp = fopen( path, "rb" ) ) ) goto End;
	if( fread( ip_address,           1, BUFSIZE_IPADDRESS, fp ) != BUFSIZE_IPADDRESS ) goto End;
	if( fread( &port_no  , sizeof(int),                  1, fp ) !=                1 ) goto End;
	if( fread( &check    ,           1,                  1, fp ) >                 0 ) goto End;

	b_ret = true;

End:

	if( fp ) fclose( fp );

	if( !b_ret )
	{
		memset( ip_address, 0, BUFSIZE_IPADDRESS );
		port_no = 0;
	}
	return b_ret;
}

bool ConnectTo::Save()
{
	char path[ MAX_PATH ];
	bool b_ret = false;

	sprintf( path, "%s\\%s", g_dir_profile, _filename );

	FILE *fp = NULL;

	if( fp = fopen( path, "wb" ) )
	{
		if( fwrite( ip_address,           1, BUFSIZE_IPADDRESS, fp ) != BUFSIZE_IPADDRESS ) goto End;
		if( fwrite( &port_no  , sizeof(int),                  1, fp ) !=                1 ) goto End;
		fclose( fp );
	}
	b_ret = true;

End:
	return b_ret;
}
