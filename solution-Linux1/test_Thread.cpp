#include <stdio.h>    // FILE / printf() / perror()
#include <pthread.h>  // pthread_xxx()
#include <unistd.h>   // usleep()

#include "DebugLog.h"

static pthread_t _th_id    =     0;
static bool      _b_cancel = false;


static void _sleep_msec( int msec )
{
	usleep( msec * 1000 );
}

static void *_Thread( void *arg )
{
	bool b_ret = false  ;

	for( int i = 0; i < 3; i++ )
	{
		printf( "th = %d\n", i );
		_sleep_msec( 500 );
	}

	b_ret = true;
End:
	
	return arg;
}

bool test_Thread( void )
{
	bool b_ret = false;
	int  th_stat;
	void *th_res;

	_b_cancel = false             ;
	th_stat   = pthread_create( &_th_id, NULL, _Thread, (void *)NULL );
	if( th_stat ){ dlog( "create thread: ERR." ); goto End; }
	else         { dlog( "create thread: OK."  );           }
	pthread_join( _th_id, &th_res );

	b_ret = true;
End:

	return b_ret;
}
