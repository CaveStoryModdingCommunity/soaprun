#include <fcntl.h>    // O_WRONLY / S_IRWXU etc..
#include <limits.h>   // PATH_MAX / NAME_MAX
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <stdarg.h>   // va_start() / va_end()
#include <stdio.h>    // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <stdlib.h>    // mode_t / exit() / malloc() / atoi()
#include <sys/stat.h> // umask() / stat() / chmod()
#include <time.h>     // time()

#include <pthread.h>  // pthread_xxx()
#include <unistd.h>   // _exit() / readlink() / unlink() / sleep() / setsid() / close() / dup2()
#define MAX_PATH (PATH_MAX+NAME_MAX)
static pthread_mutex_t _mutex;


static char            _directory[ MAX_PATH ] = "";
static bool            _bValid                = false;
static bool            _bDaemon               = false;
static char            _last_log[ 1000 ];
static const char*     _directory_name = "debuglog";


// time
static struct tm* _Get_tm( void )
{
	time_t             time_value;
	time(             &time_value );
	return localtime( &time_value );
}

bool DebugLog_Initialize( bool bDaemon )
{
	char   path[ MAX_PATH ];
	mode_t mode = S_IRWXU  | S_IRWXG | S_IRWXO;
	long   i;

	_bDaemon = bDaemon;

	// work directory
	readlink( "/proc/self/exe", _directory, MAX_PATH );

	for( i = strlen( _directory ) -1; i > 0; i-- )
	{
		if( _directory[ i ] == '/' ){ _directory[ i ] = '\0'; break; }
	}
	if( i == 0 ) return false;
	sprintf( _directory, "%s/%s", _directory, _directory_name );

	mkdir(   _directory, mode );
	chmod(   _directory, 0777 );

	memset( &_mutex, 0, sizeof(pthread_mutex_t) );
	if( pthread_mutex_init( &_mutex, NULL ) ) return false;

	_bValid = true;

	return true;
}

void DebugLog_Release( void )
{
	if( !_bValid ) return;

	pthread_mutex_destroy( &_mutex );

	_bValid = false;
}

void dlog( const char* format, ... )
{
	if( !_bValid ) return;

	if( !pthread_mutex_lock( &_mutex ) && _bValid )
	{

		va_list    ap              ;
		char       str[   1024    ];
		char       path[ MAX_PATH ];
		struct tm* p_tm = _Get_tm();
		FILE*      fp              ;

		va_start( ap , format     );
		vsprintf( str, format, ap );
		va_end(                ap );

		if( strcmp( _last_log, str ) )
		{
			strcpy( _last_log, str ); if( !_bDaemon ) printf( "%s\n", str );

			sprintf( path, "%s/%04d%02d%02d", _directory, p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday );

			if( fp = fopen( path, "a+t" ) )
			{
				fprintf( fp, "%02d%02d%02d,%s\n", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec, str );
				fclose( fp );
				chmod( path, 0777 );
			}
		}
		pthread_mutex_unlock( &_mutex );
	}
}

#include <dirent.h> // dirent

bool DebugLog_Store( const char* store_directory )
{
	bool bReturn = false;

	if( !_bValid ) return bReturn;

	if( !pthread_mutex_lock( &_mutex ) && _bValid )
	{

	    DIR*           dir                 ;
		struct dirent* dirEnt              ;
		char           path_src[ MAX_PATH ];
		char           path_dst[ MAX_PATH ];
		struct tm*     p_tm = _Get_tm()    ;

	    if( dir = opendir( _directory ) )
		{

			while( dirEnt = readdir( dir ) )
			{
				if( strcmp( dirEnt->d_name, "." ) && strcmp( dirEnt->d_name, ".." ) )
				{
					sprintf( path_src, "%s/%s"                          , _directory, dirEnt->d_name );			
					sprintf( path_dst, "%s/%04d%02d%02d-%02d%02d%02d-%s", _directory,
							p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
							p_tm->tm_hour       , p_tm->tm_min    , p_tm->tm_sec , dirEnt->d_name );
					rename(  path_src, path_dst );
					chmod(   path_dst, 0777 );
				}
			}
			closedir( dir );

			bReturn = true;
		}
		pthread_mutex_unlock( &_mutex );
	}

	return bReturn;
}

