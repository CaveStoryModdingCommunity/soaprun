#include <stdio.h>    // FILE / printf() / perror()
#include <limits.h>   // PATH_MAX / NAME_MAX
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <unistd.h>   // usleep()
#include <pthread.h>  // pthread_xxx()


#define MAX_PATH (PATH_MAX+NAME_MAX)

#define _MAXATTENDER 10

#include "svAttender.h"
#include "DebugLog.h"

static void _sleep_msec( int msec ){ usleep( msec * 1000 ); }

static void *svAttender_Thread( void *arg )
{
	bool b_ret = false  ;

	svAttender *atnd = (svAttender*)arg;

	for( int i = 0; i < 3; i++ )
	{
		printf( "atnd[ %d ] = %d\n",atnd->No(), i );
		_sleep_msec( 500 );
	}

	b_ret = true;
End:
	
	return arg;
}


svAttender::svAttender( int no )
{
	_status = _ATTENDER_IDLE;
	_sock   =              0;
	_thrd   =              0;
	_no     = no            ;

	int  th_stat;

	th_stat   = pthread_create( &_thrd, NULL, svAttender_Thread, (void *)this );
	if( th_stat ){ dlog( "thrd attender: ERR." ); _status = _ATTENDER_ERROR; return; }
	else         { dlog( "thrd attender: OK."  );         }
}

svAttender::~svAttender()
{

}

bool svAttender::WaitExit()
{
	void *th_res;
	pthread_join( _thrd, &th_res );
	return true;
}


int svAttender::No()
{
	return _no;
}
