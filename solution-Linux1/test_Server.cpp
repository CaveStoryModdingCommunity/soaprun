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

#define _PORTNO 1001


class lnxServer
{
private:

	int _sock_sv;

public :

	lnxServer()
	{
		_sock_sv = 0;
	}
	~lnxServer()
	{
		if( _sock_sv ) close( _sock_sv );
	}

	bool BindAndListen( int port_no )
	{
		bool               b_ret = false;
		struct sockaddr_in addr;

		dlog( "bind and listen." );

		if( (_sock_sv = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) goto End;
		dlog( "socket: ok." );

		memset( &addr, 0, sizeof(addr) );

		addr.sin_family      = PF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port        = htons( port_no  );

		if( bind( _sock_sv, (struct sockaddr *)&addr, sizeof(addr) ) < 0 ) goto End;
		dlog( "bind: ok."   );

		if( listen( _sock_sv, 5 ) < 0 ) goto End;
		dlog( "listen: ok." );

		b_ret = true;
End:

		return b_ret;
	}

	bool Accept( int *p_sock_cl )
	{
		struct sockaddr_in addr      ;
		socklen_t          writer_len;
		if( ( *p_sock_cl = accept( _sock_sv, (struct sockaddr *)&addr, &writer_len ) ) < 0 ) return false;
		dlog( "accept: ok %s", inet_ntoa( addr.sin_addr ) );
		return true;
	}
};

bool test_Server( void )
{
	bool   b_ret = false;

	char buf[ 100 ];

	lnxServer sv;
	int sock_cl;

	if( !sv.BindAndListen( _PORTNO ) ) goto End;

	if( !sv.Accept( &sock_cl ) ) goto End;

	if( !lnxSock_Recv( buf, 100, sock_cl, true ) ) goto End;

	strcpy( buf, "Hello, Sorprun!" );

	if( !lnxSock_Send( buf, strlen( buf ) + 1,  sock_cl, true ) ) goto End;


	b_ret = true;
End:
	
	dlog( "errno: %d", errno );

	return b_ret;
}

