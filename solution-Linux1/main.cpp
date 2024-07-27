#include <stdio.h>    // FILE / printf() / perror()
#include <limits.h>   // PATH_MAX / NAME_MAX
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()

#define MAX_PATH (PATH_MAX+NAME_MAX)

#include "Daemon.h"
#include "DebugLog.h"
#include "Signal.h"


const char        *g_version_text = "v.0.0.0.3" ;
const char        *g_date_text    = "2010/01/27";

static const char *_app_name      = "Linux1";


//bool test_Server( void );
//bool test_Thread( void );
bool Loop_Main();


int main( int argc, char* argv[] )
{
	bool bDaemon = false;

	// option..
	if( argc > 1 )
	{
		if(      !strcmp( argv[ 1 ], "-D"  ) )
		{
			printf( "%s %s (%s) can't daemon\n", _app_name, g_version_text, g_date_text );
			goto End;
			bDaemon = true;
		}
		else if( !strcmp( argv[ 1 ], "--version" ) )
		{
			printf( "%s %s (%s)\n", _app_name, g_version_text, g_date_text );
			goto End;
		}
	}

	// debug log
	if( !DebugLog_Initialize( bDaemon ) ) return 1;
	if( argc > 1 ) dlog( "argv[ 1 ] : %s", argv[ 1 ] );

	dlog( "## start %s %s (%s) ##", _app_name, g_version_text, g_date_text );

	if( bDaemon && !Daemon_Set() ) goto End;

	Signal_Set(  ); // signal

	Loop_Main(   ); // main loop

//	test_Thread();
//	test_Server();

	dlog( "normal exit" );

End:

	DebugLog_Release();

	return 0;
}
