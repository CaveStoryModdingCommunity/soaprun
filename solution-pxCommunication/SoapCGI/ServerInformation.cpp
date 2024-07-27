#include <StdAfx.h>

#include "ServerInformation.h"

bool ServerInformation_Save( const char *path_dst, const SERVERINFORMATION *p )
{
	bool b_ret = false;
	FILE *fp   = NULL ;

	if( !( fp = fopen( path_dst, "wb" ) ) ) goto End;

	if( fwrite( p, sizeof(SERVERINFORMATION), 1, fp ) != 1 ) goto End;

	b_ret = true;
End:

	if( fp ) fclose( fp );
	return b_ret;
}

bool ServerInformation_Load( const char *path_src, SERVERINFORMATION *p )
{
	bool b_ret = false;
	FILE *fp   = NULL ;

	if( !( fp = fopen( path_src, "rb" ) ) ) goto End;

	if( fread( p, sizeof(SERVERINFORMATION), 1, fp ) != 1 ) goto End;

	b_ret = true;
End:

	if( !b_ret ) memset( p, 0, sizeof(SERVERINFORMATION) );

	if( fp ) fclose( fp );

	return b_ret;
}
