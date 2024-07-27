#include <StdAfx.h>

#include "wsServer.h"

wsServer::wsServer()
{
	_b_valid     = false;
	_sock        = INVALID_SOCKET;
	_hEvent_Exit = CreateEvent( NULL, TRUE, FALSE, NULL );
	memset( _host_name, 0, MAX_HOSTNAME );
	memset( _host_ip  , 0, MAX_HOSTNAME );

	memset( &_client, 0, sizeof(HOSTSTRUCT) );
	_client.sock = INVALID_SOCKET;
}

wsServer::~wsServer()
{
	if( _hEvent_Exit ){ SetEvent( _hEvent_Exit ); CloseHandle( _hEvent_Exit ); _hEvent_Exit = NULL; }
	if( _sock != INVALID_SOCKET ) closesocket( _sock );
}

bool wsServer::BindAndListen( unsigned short port )
{
	bool b_ret = false;

	if( 0 )
	{
		if( gethostname( _host_name, MAX_HOSTNAME ) == SOCKET_ERROR ) goto End;
		printf( "[sv] name: %s\n", _host_name );

		// ip address
		struct in_addr addr;
		struct hostent *hs ;
		if( !(hs = gethostbyname( _host_name )) ) goto End;

		for( long i = 0; hs->h_addr_list[ i ]; i++ )
		{
			addr.S_un.S_addr = *(unsigned long*)hs->h_addr_list[ i ];
			if( !i ) strcpy( _host_ip, inet_ntoa( addr ) );
			printf( "[sv] ip: %s\n",   inet_ntoa( addr ) );
		}
	}

	if( ( _sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET ) goto End;
	printf( "[sv] socket: ok\n" );

	{
		SOCKADDR_IN sockaddr;

		memset( &sockaddr, 0, sizeof(sockaddr) );
		sockaddr.sin_port        = htons( port );
		sockaddr.sin_family      = AF_INET;
		sockaddr.sin_addr.s_addr = INADDR_ANY;

		if( bind  ( _sock, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR) ) == SOCKET_ERROR ) goto End;
		printf( "[sv] bind: ok\n"   );

		if( listen( _sock, 1 ) == SOCKET_ERROR ) goto End;
		printf( "[sv] listen: ok\n" );
	}

	b_ret = true;
End:
	return b_ret;
}

bool wsServer::Accept()
{
	SOCKADDR_IN sockaddr;
	int         addr_size = sizeof(SOCKADDR_IN);

	memset( &sockaddr, 0, sizeof(sockaddr) );

	_client.sock = accept( _sock, (SOCKADDR *)&sockaddr, &addr_size );
	if( _client.sock == INVALID_SOCKET ) return false;
	strcpy( _client.host_name, inet_ntoa( sockaddr.sin_addr ) );
	return true;
}

const char* wsServer::ClientName( void )
{
	if( _client.sock == INVALID_SOCKET ) return "?";
	return _client.host_name;
}

int wsServer::Send( const char *p_buf, int data_size, const bool *p_b_stop )
{
	return lnxSock_Send( p_buf, data_size, _client.sock, p_b_stop );
}

int  wsServer::Recv( char *p_buf, int max_recv, bool b_sizehead, const bool *p_b_stop )
{
	return lnxSock_Recv( p_buf, max_recv, _client.sock, b_sizehead, p_b_stop );
}

bool wsServer::Shutdown( void )
{
	return lnxSock_Close( &_client.sock );
}
