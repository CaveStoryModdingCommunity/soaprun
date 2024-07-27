#pragma once

#define MAX_HOSTNAME 128

//typedef BOOL ( * func_STARTED )( void* p ); // 引数はサーバ(クライアント)構造体のポインタ

typedef struct
{
	bool   b_valid;
	char   host_name[ MAX_HOSTNAME ];
	HANDLE hEvent_Exit;
	SOCKET sock;
}
HOSTSTRUCT;

bool ws_Initialize( void );
void ws_Release( void );

//int  lnxSock_Recv ( char       *buf, int buf_size , int sock, bool b_sizehead );
//bool lnxSock_Send ( const char *buf, int data_size, int sock, bool b_sizehead );
int  lnxSock_Recv( char       *buf, int buf_size , int sock, bool b_sizehead, const bool *p_b_stop );
//bool lnxSock_Send( const char *buf, int data_size, int sock, bool b_sizehead, const bool *p_b_stop );
int lnxSock_Send     ( const char *p   , int data_size, int sock, const bool *p_b_stop );
int lnxSock_Send_mbuf(       char *mbuf, int data_size, int sock, const bool *p_b_stop );

//int  lnxSock_LastRecv( char *p, int buf_size, int sock, const bool *p_b_stop, int sec_timeout );
bool lnxSock_Close( SOCKET *p_sock );
