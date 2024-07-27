#include "StdAfx.h"


static CRITICAL_SECTION _cs;

static bool        _b_cs     = false;
static const char* _path_dir = NULL;
static char        _backup_str[ 1024 ];

static const char* _ext = ".dbg";

void DebugLog_Initialize( const char* path_dir )
{
	_path_dir = path_dir;

	InitializeCriticalSection( &_cs );

	_b_cs = true;
}

void DebugLog_Release( void )
{
	if( _b_cs ) DeleteCriticalSection( &_cs );
	_b_cs = false;
}


void dlog( const char *fmt, ... )
{
	if( !_path_dir || !_b_cs ) return;

	EnterCriticalSection( &_cs );
	{
		SYSTEMTIME st;
		char       str[512];

		GetLocalTime( &st );
		sprintf( str, "%s\\%04d%02d%02d%s", _path_dir, st.wYear, st.wMonth, st.wDay, _ext );

		FILE* fp = fopen( str, "a+t" );

		if( fp )
		{
			va_list ap; va_start( ap, fmt ); vsprintf( str, fmt, ap ); va_end( ap );

//			if( strcmp( str, _backup_str ) )
//			{
				fprintf( fp, "%02d%02d%02d,%s\n", st.wHour, st.wMinute, st.wSecond, str );
				strcpy( _backup_str, str );
//			}
			fclose( fp );
		}
	}
	LeaveCriticalSection( &_cs );
}

union _TIME
{
	DWORDLONG dwlong;
	FILETIME  ft;
};

#define _1HOUR (DWORDLONG)( 60 * (DWORDLONG)(60 * (1000 * 10000)) )

static int _GetNameOld( const char* name )
{
	if( strlen( name ) - strlen( _ext ) != 8 ) return -1;

	for( int i = 0; i < 8; i++ )
	{
		if( name[ i ] < '0' || name[ i ] > '9' ) return -1;
	}

	SYSTEMTIME st;
	_TIME      time_now ;
	_TIME      time_name;

	GetLocalTime(         &st );
	SystemTimeToFileTime( &st, &time_now.ft );

	st.wYear  = (WORD)( name[ 0 ] - '0' ) * 1000 +
				(WORD)( name[ 1 ] - '0' ) *  100 +
				(WORD)( name[ 2 ] - '0' ) *   10 +
				(WORD)( name[ 3 ] - '0' ) *    1 ;
	st.wMonth = (WORD)( name[ 4 ] - '0' ) *   10 +
				(WORD)( name[ 5 ] - '0' ) *    1 ;
	st.wDay   = (WORD)( name[ 6 ] - '0' ) *   10 +
				(WORD)( name[ 7 ] - '0' ) *    1 ;

	SystemTimeToFileTime( &st, &time_name.ft );


	if( time_name.dwlong > time_now.dwlong ) return -1;

	return (int)( ( time_now.dwlong - time_name.dwlong ) / ( 24 * _1HOUR ) );
}

int DebugLog_DeleteOlds( int days )
{
	HANDLE          hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA find;
	int             count = 0;
	char            path[ MAX_PATH ];

	sprintf( path, "%s\\*%s", _path_dir, _ext );

	if( ( hFind = FindFirstFile( path, &find ) ) == INVALID_HANDLE_VALUE ) return 0;

	do
	{
		if( _GetNameOld( find.cFileName ) >= days )
		{
			sprintf( path, "%s\\%s", _path_dir, find.cFileName );
			if( DeleteFile( path ) ) count++;
		}
	}
	while( FindNextFile( hFind, &find ) );
	FindClose( hFind );

	return count;
}

