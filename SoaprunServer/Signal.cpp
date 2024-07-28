#include <stdio.h>    // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <pthread.h>  // pthread_xxx()

#include "DebugLog.h"
#include "Signal.h"

volatile sig_atomic_t g_exit = 0;
static pthread_t      _thrd  = 0;

bool Signal_Mask( sigset_t *p_ss )
{
	sigset_t ss;
	if( !p_ss ) p_ss = &ss;
	
	sigemptyset  ( p_ss         );
	if( sigaddset( p_ss, SIGINT ) ) return false;
	if( pthread_sigmask( SIG_BLOCK, p_ss, NULL ) ) return false;
	return true;
}

static void *thrd_Signal( void *arg )
{
	sigset_t ss    ;
	int      sig_no;

	if( !Signal_Mask( &ss ) ) return 0;
	
	while( !g_exit )
	{
		if( !sigwait( &ss, &sig_no ) )
		{
			switch( sig_no )
			{
			case SIGINT:
				printf( "sig[ int ]\n" );
				g_exit = 1;
				break;

			default    :
				printf( "sig[ ? ]\n"   );
				g_exit = 2;
				break;
			}
		}
		else
		{
			printf( "sigwait: Err." );
			return 0;
		}
	}
}

bool Signal_CreateThread()
{
	if( pthread_create( &_thrd, 0, thrd_Signal, 0 ) ) return false;
	return true;
}


bool Signal_JoinThread()
{
	int *status;
	pthread_join( _thrd, (void **)&status );
	return true;
}
