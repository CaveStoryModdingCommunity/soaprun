#include <stdio.h>      // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()

#ifdef NDEBUG

//#include <windows.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SHUT_RDWR SD_BOTH

static void _sleep_msec( int msec ){ Sleep( msec ); }

#else

#include <unistd.h>     // close()
#include <string.h>     // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <arpa/inet.h>  // inet_addr() / inet_ntoa()
#include <sys/socket.h> // shutdown()
#include <errno.h>      // errno

static void _sleep_msec( int msec ){ usleep( msec * 1000 ); }

#endif

#include "SharedDefines.h"
#include "DebugLog.h"
#include "lnxSock.h"
#include "LinkUnits.h"
#include "svListener.h"
#include "Signal.h"

extern LinkUnits *g_link_units;


static void *_Thread( void *arg )
{
	bool       b_ret   = false;
	int        sock_cl =     0;
	svListener *lstn   = (svListener*)arg;
	char       ip_address[ BUFFERSIZE_IPADDRESS ];
//	char       buf[ 8 ];

#ifndef NDEBUG
	if( !Signal_Mask( NULL )   ) goto End;
#endif

	if( !lstn->BindAndListen() ) goto End;

	while( !g_exit )
	{
		if( lstn->Accept( &sock_cl, ip_address ) )
		{
			if( !g_link_units->Push( ip_address, "-", sock_cl ) )
			{
				dlog( "Over Link: %s", ip_address );
				close( sock_cl );
			}
		}
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


#ifdef NDEBUG
#else

	int  th_stat;
	th_stat   = pthread_create( &_thrd, NULL, _Thread, (void *)this );
	if( th_stat ){ dlog( "thrd listner: ERR." ); _status = _LISTENER_ERR_THREAD; return; }
	else         { dlog( "thrd listner: OK."  );         }

#endif

}

svListener::~svListener()
{
	if( _sock ) close( _sock );
}

bool svListener::WaitExit()
{

#ifdef NDEBUG
#else

	void *th_res;
	pthread_join( _thrd, &th_res );

#endif

	return true;
}


bool svListener::BindAndListen()
{
	bool               b_ret = false;
	struct sockaddr_in addr;

	dlog( "bind and listen. port: %d", _port_no );

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

bool svListener::Accept( int *p_sock_cl, char *ip_address )
{
	struct sockaddr_in addr;
	socklen_t          writer_len = sizeof(struct sockaddr_in);

	struct timeval   tv = { 0, 0 };
	fd_set           rs  ;
	FD_ZERO(        &rs );
	FD_SET ( _sock, &rs );

	int rc = select( _sock + 1, &rs, NULL, NULL, &tv );
	if( rc <  0 ) perror("accept-select error");
	if( rc == 0 ) return false;

	memset( &addr, 0, sizeof(struct sockaddr_in) );
	if( ( *p_sock_cl = accept( _sock, (struct sockaddr *)&addr, &writer_len ) ) < 0 )
	{
		_status = _LISTENER_ERR_ACCEPT;
		return false;
	}

	strcpy( ip_address, inet_ntoa( addr.sin_addr ) );
//	dlog( "accept: sock=%d, ip=%s", *p_sock_cl, ip_address );
	return true;
}

bool svListener::Kill()
{
	if( !_sock ) return false;

	
	if( shutdown( _sock, SHUT_RDWR ) ) dlog( "listen shut: " "errno = %d!", errno );
	if( close   ( _sock            ) ) dlog( "listen close: ""errno = %d!", errno );
	printf( "close listen.\n" );
	_sock = 0;
	return true;
}
