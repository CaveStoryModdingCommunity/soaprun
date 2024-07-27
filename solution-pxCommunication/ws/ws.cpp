#include <StdAfx.h>

#include "../Fixture/DebugLog.h"
#include "ws.h"


bool ws_Initialize( void )
{
	WORD    version_requested = WINSOCK_VERSION;
	WSADATA wsa_data;

	if( WSAStartup( version_requested, &wsa_data ) ){               return false; }
	if( wsa_data.wVersion != version_requested     ){ WSACleanup(); return false; }

	return true;
}

void ws_Release( void )
{
	WSACleanup();
}

//#define BUFFERSIZE_SEND 1024

#define _SELECTINTERVAL 10


static int _Recv( char *p, int buf_size, int sock, const bool *p_b_stop, bool b_once )
{
	int            count = 0;
	int            ok_size  ;
	fd_set         rs       ;
	struct timeval tv       ;

	while( 1 )
	{
		FD_ZERO(       &rs );
		FD_SET ( sock, &rs );
		tv.tv_sec = tv.tv_usec = 0;

		int rc = select( sock + 1, &rs, NULL, NULL, &tv );
		if( rc <  0 ){ dlog( "select: Err! %d", rc ); return -1; }
		if( rc != 0 && FD_ISSET( sock, &rs ) )
		{
			ok_size = recv( sock, p,      buf_size - count, 0 );
			if( ok_size <= 0 || ok_size > buf_size - count ){ dlog( "recv: Err! %d", ok_size ); return -1; }
			p      += ok_size;
			count  += ok_size;
			if( b_once || count >= buf_size ) break;
		}
		if( *p_b_stop ){ dlog( "recv: Exit." ); return -1; }
		Sleep( _SELECTINTERVAL );
	}

	return count;
}

int lnxSock_Send( const char *p, int data_size, int sock, const bool *p_b_stop )
{
	int            count = 0;
	int            ok_size  ;
	fd_set         ws       ;
	struct timeval tv       ;

	while( 1 )
	{
		FD_ZERO(       &ws );
		FD_SET ( sock, &ws );
		tv.tv_sec = tv.tv_usec = 0;

		int rc = select( sock + 1, NULL, &ws, NULL, &tv );
		if( rc <  0 ){ dlog( "select: Err! %d", rc ); return -1; }
		if( rc != 0 && FD_ISSET( sock, &ws ) )
		{
			ok_size = send( sock, p,      data_size - count, 0 );
			if( ok_size <= 0 || ok_size > data_size - count ){ dlog( "send: Err! %d", ok_size ); return -1; }
			p      += ok_size;
			count  += ok_size;
			if( count >= data_size ) break;
		}
		if( *p_b_stop ){ dlog( "send: Exit." ); return -1; }
		Sleep( _SELECTINTERVAL );
	}

	return count;
}

int lnxSock_Send_mbuf( char *mbuf, int data_size, int sock, const bool *p_b_stop )
{
	char *p = (char*)( mbuf - sizeof(int) );
	*(int*)p = data_size;
	return lnxSock_Send( p, data_size + sizeof(int), sock, p_b_stop );
}









/*

int lnxSock_LastRecv( char *p, int buf_size, int sock, const bool *p_b_stop, int sec_timeout )
{
	int            count = 0;
	int            ok_size  ;
	fd_set         rs       ;
	struct timeval tv       ;

	for( int i = 0; i < sec_timeout; i++ )
	{
		FD_ZERO(       &rs );
		FD_SET ( sock, &rs );
		tv.tv_sec = tv.tv_usec = 0;

		int rc = select( sock + 1, &rs, NULL, NULL, &tv );
		if( rc <  0 ){ dlog( "select: Err! %d", rc ); return -1; }
		if( rc != 0 && FD_ISSET( sock, &rs ) )
		{
			ok_size = recv( sock, p,      buf_size - count, 0 );
			if( ok_size <= 0 || ok_size > buf_size - count ){ dlog( "recv: Err! %d", ok_size ); return -1; }
			p      += ok_size;
			count  += ok_size;
			if( count >= buf_size ) break;
		}
		if( *p_b_stop ){ dlog( "recv: Exit." ); return -1; }
		Sleep( 1000 );
	}

	return count;
}
*/

// ------
// global
// ------

int lnxSock_Recv( char *buf, int buf_size, int sock, bool b_sizehead, const bool *p_b_stop )
{
	int recv_size;

	if( b_sizehead ){ if( _Recv( (char*)&recv_size, sizeof(int), sock, p_b_stop, false ) != sizeof(int) ) return -1; }
	else            { recv_size = buf_size; }

	if( !recv_size ) return 0;

	int ok_size = _Recv( buf, recv_size, sock, p_b_stop, b_sizehead ? false : true );

	if( b_sizehead && ok_size != recv_size ) return -1;

	return ok_size;
}
/*
bool lnxSock_Send( const char *buf, int data_size, int sock, bool b_sizehead, const bool *p_b_stop )
{
	if( b_sizehead )
	{
		if( _Send( (char*)&data_size, sizeof(int), sock, p_b_stop ) != sizeof(int) ) return false;
		if( !data_size ) return true;
	}

	int ok_size = _Send( buf, data_size, sock, p_b_stop );

	if( b_sizehead && ok_size != data_size ) return false;

	return true;
}
*/












bool lnxSock_Close( SOCKET *p_sock )
{
	bool b_ret = false;
	if( shutdown   ( *p_sock , SD_BOTH ) ){ dlog( "shutdown: err!"  ); goto End; }
	if( closesocket( *p_sock           ) ){ dlog( "closesock: err!" ); goto End; }
	dlog( "closesock: ok." );
	b_ret = true;
End:
	*p_sock = INVALID_SOCKET;
	return b_ret;
}
