#include <fcntl.h>    // O_WRONLY / S_IRWXU etc..
#include <limits.h>   // PATH_MAX / NAME_MAX
#include <stdio.h>    // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()

#include <dirent.h>   // dirent
#include <unistd.h>   // _exit() / readlink() / unlink() / sleep() / setsid() / close() / dup2()
#include <sys/stat.h> // mkdir()

#define MAX_PATH (PATH_MAX+NAME_MAX)

bool Directory_Create( const char* name, char* path, int size )
{
	char   path_module[ MAX_PATH ];
	mode_t mkdir_mode = S_IRWXU  | S_IRWXG | S_IRWXO;
	int    i;

	memset(  path       , 0, size     );
	memset(  path_module, 0, MAX_PATH );
	readlink( "/proc/self/exe", path_module, MAX_PATH );

	for( i = strlen( path_module ) -1; i > 0; i-- )
	{
		if( path_module[ i ] == '/' ){ path_module[ i ] = '\0'; break; }
	}
	if( i == 0 ) return false;

	sprintf( path, "%s/%s", path_module, name );
	mkdir(   path, mkdir_mode );
	chmod(   path, 0777 );

	return true;
}

long Directory_ClearFiles( const char* directory )
{
	DIR*           dir     = NULL  ;
	struct dirent* dirEnt          ;
	char           path[ MAX_PATH ];

	dir = opendir( directory ); if( !dir ) return -1;

	while( dirEnt = readdir( dir ) )
	{
		if( strcmp( dirEnt->d_name, "." ) && strcmp( dirEnt->d_name, ".." ) )
		{
			sprintf( path, "%s/%s", directory, dirEnt->d_name );
			unlink(  path );
		}
	}

	closedir( dir );
}

bool Directory_Delete( const char* directory )
{
	Directory_ClearFiles( directory );
	if( rmdir( directory ) ) return false;
	return true;
}
