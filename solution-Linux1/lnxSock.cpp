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


#define BUFFERSIZE_RECV 1024
#define BUFFERSIZE_SEND 1024

int lnxSock_Recv( void *buf, int buf_size, int sock, bool b_sizehead )
{
	int ok_size;
	int recv_size;

	if( buf_size > BUFFERSIZE_RECV ) return -1;

	if( b_sizehead )
	{
		if( recv( sock, (char*)&recv_size, 4, 0 ) != 4 ) return -1;
		dlog( "recv size: %d byte", recv_size );
		if( recv_size > buf_size ) return false;
	}

	ok_size = recv( sock, buf, recv_size, 0 );
	if( b_sizehead ){ if( ok_size != recv_size ) return   -1; }
	dlog( "recv ok: %d byte ok.", ok_size );

	return ok_size;
}

bool lnxSock_Send( void *buf, int data_size, int sock, bool b_sizehead )
{

	char send_buf[ BUFFERSIZE_SEND + 4 ];
	int  send_size = data_size;
	int  ok_size;

	if( data_size > BUFFERSIZE_SEND ) return false;

	if( b_sizehead )
	{
		*(int*)send_buf = data_size;
		memcpy( &send_buf[ 4 ], buf, data_size );
		send_size = data_size + 4;
	}
	else
	{
		memcpy( &send_buf[ 0 ], buf, data_size );
	}

	dlog( "send size: %d byte", send_size );
	ok_size = send( sock, send_buf, send_size, 0 );
	dlog( "send ok: %d byte"  , ok_size );
	if( ok_size != send_size ) return false;

	return true;
}
