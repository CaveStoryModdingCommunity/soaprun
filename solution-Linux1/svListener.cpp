#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

#include <unistd.h> // close()
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <arpa/inet.h> // inet_addr() / inet_ntoa()

#include <errno.h>

#include "DebugLog.h"
#include "lnxSock.h"
#include "svListener.h"

#define _PORTNO 1001

static void _sleep_msec( int msec ){ usleep( msec * 1000 ); }

static void *svListener_Thread( void *arg )
{
	bool b_ret   = false;
	int  sock_cl =     0;

	svListener *lstn = (svListener*)arg;

	if( !lstn->BindAndListen() ) goto End;

	for( int i = 0; i < 5; i++ )
	{
		if( !lstn->Accept( &sock_cl ) ) goto End;

		char buf[ 100 ];

		if( !lnxSock_Recv( buf, 100, sock_cl, true ) ) goto End;
		sprintf( buf, "Hello, Sorprun! %d", i );
		if( !lnxSock_Send( buf, strlen( buf ) + 1,  sock_cl, true ) ) goto End;
		close( sock_cl );

		_sleep_msec( 1000 );
	}

	b_ret = true;
End:
	
	return arg;
}

svListener::svListener( int port_no )
{
	_status  = _LISTENER_IDLE;
	_sock    =              0;
	_thrd    =              0;
	_port_no = port_no       ;

	int  th_stat;

	th_stat   = pthread_create( &_thrd, NULL, svListener_Thread, (void *)this );
	if( th_stat ){ dlog( "thrd listner: ERR." ); _status = _LISTENER_ERR_THREAD; return; }
	else         { dlog( "thrd listner: OK."  );         }
}

svListener::~svListener()
{
	if( _sock ) close( _sock );
}

bool svListener::WaitExit()
{
	void *th_res;
	pthread_join( _thrd, &th_res );
	return true;
}


bool svListener::BindAndListen()
{
	bool               b_ret = false;
	struct sockaddr_in addr;

	dlog( "bind and listen." );

	if( (_sock = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) goto End;
	dlog( "socket: ok." );

	memset( &addr, 0, sizeof(addr) );

	addr.sin_family      = PF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port        = htons( _port_no );

	if( bind( _sock, (struct sockaddr *)&addr, sizeof(addr) ) < 0 ) goto End;
	dlog( "bind: ok."   );

	if( listen( _sock, 5 ) < 0 ) goto End;
	dlog( "listen: ok." );

	b_ret = true;
End:
	if( !b_ret ) _status = _LISTENER_ERR_LISTEN;

	return b_ret;
}

bool svListener::Accept( int *p_sock_cl )
{
	struct sockaddr_in addr      ;
	socklen_t          writer_len = sizeof(struct sockaddr_in);

	memset( &addr, 0, sizeof(struct sockaddr_in) );
	if( ( *p_sock_cl = accept( _sock, (struct sockaddr *)&addr, &writer_len ) ) < 0 )
	{
		_status = _LISTENER_ERR_ACCEPT;
		return false;
	}

	dlog( "accept: ok %s", inet_ntoa( addr.sin_addr ) );
	return true;
}

bool svListener::Kill()
{
	if( !_sock ) return false;
	close( _sock );
	_sock = 0;
	return true;
}
