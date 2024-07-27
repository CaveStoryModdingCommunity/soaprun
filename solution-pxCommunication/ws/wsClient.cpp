#include <StdAfx.h>

#include "../Fixture/DebugLog.h"
#include "wsClient.h"

wsClient::wsClient()
{
	memset( &_host, 0, sizeof(_host) );
	_host.sock         = INVALID_SOCKET;
	_host.hEvent_Exit = CreateEvent( NULL, TRUE, FALSE, NULL );

	memset( server_address, 0, MAX_HOSTNAME );
	server_port = 0;
}

wsClient::~wsClient()
{
	if( _host.hEvent_Exit ){ SetEvent( _host.hEvent_Exit ); CloseHandle( _host.hEvent_Exit ); _host.hEvent_Exit = NULL; }
	if( _host.sock != INVALID_SOCKET ) lnxSock_Close( &_host.sock );
}

bool wsClient::Connect( const char *ip_address, unsigned short port, int *p_wsa_err )
{
	bool b_ret = false;

	if( ( _host.sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET ) goto End;

	in_addr sv_addr  ;

	memset( &sv_addr, 0, sizeof(sv_addr) );
	if( ( sv_addr.s_addr = inet_addr( ip_address ) ) == INADDR_NONE )
	{
		LPHOSTENT host;
		if( !( host = gethostbyname( ip_address ) ) ) goto End;
		sv_addr = *( (LPIN_ADDR)*host->h_addr_list );
	}

	{
		SOCKADDR_IN sockaddr;

		memset( &sockaddr, 0, sizeof(SOCKADDR_IN) );
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr   = sv_addr;
		sockaddr.sin_port   = htons( port );

		// ê⁄ë±ÇóvãÅÇ∑ÇÈ
		if( connect( _host.sock, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR_IN) ) == SOCKET_ERROR )
		{
			int res = WSAGetLastError();
			if( res != WSAEWOULDBLOCK && p_wsa_err ) *p_wsa_err = res;
			goto End;
		}
	}

	strcpy( server_address, ip_address );
	server_port = port;

	b_ret  = true;
End:
	return b_ret;
}

int  wsClient::Send( const char *p_buf, int data_size, const bool *p_b_stop )
{
	return lnxSock_Send( p_buf, data_size, _host.sock, p_b_stop );
}

int  wsClient::Send_mbuf( char *mbuf, int data_size, const bool *p_b_stop )
{
	return lnxSock_Send_mbuf( mbuf, data_size, _host.sock, p_b_stop );
}

int  wsClient::Recv( char *p_buf, int max_recv, bool b_sizehead, const bool *p_b_stop )
{
	return lnxSock_Recv( p_buf, max_recv, _host.sock, b_sizehead, p_b_stop );
}

//bool wsClient::Shutdown( char *p_buf, int max_recv, const bool *p_b_stop, int sec_timeout )
bool wsClient::Shutdown()
{
//	lnxSock_LastRecv( p_buf, max_recv, _host.sock, p_b_stop, sec_timeout );
	return lnxSock_Close( &_host.sock );
}
