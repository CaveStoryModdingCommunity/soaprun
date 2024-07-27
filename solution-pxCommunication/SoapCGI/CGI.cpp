#include <StdAfx.h>

#include "../Fixture/DebugLog.h"
#include "../Fixture/URL.h"

#include "../ws/wsClient.h"

#include "TextReader.h"
#include "CGI.h"



#ifdef _DEBUG

	static const char *_host_name = "localhost";
	static const char *_directory = "/cgi-bin/en_x0_kb/";
	static short      _port       = 8080;

#else

	static const char *_host_name = "any-provider.com";
	static const char *_directory = "/rochet/en_x0_kb/";
	static short      _port       = 80;

#endif

static const char  *_cgi_server = "server.cgi";
static const char  *_pixel_mark = "Pixel"     ;
static const char  *_ok_mark    = "OK."       ;
static const char  *_agent_name = "Soaprun1"  ;

class _Buffer
{
public:
	char *p  ;
	int  size;
	 _Buffer( int size ){ p = (char*)malloc( size ); _Buffer::size = size; memset( p, 0, size ); }
	~_Buffer( void     ){ if( p ) free( p ); p = NULL; size = 0; }
};

static _Buffer *_buf = NULL;

//--------
// Request Parameter
//--------

typedef struct
{
	const char *key  ;
	const char *param;
}
_REQUESTPARAMETER;

static void _MakeRequest( char *buf, const char *cgi_name, int req_num, const _REQUESTPARAMETER *reqs )
{
	strcpy( buf, "GET " );

	char param[ BUFFERSIZE_ENCODE + 1 ];

	strcat( buf, _directory ); strcat( buf, cgi_name );

	for( int r = 0; r < req_num; r++ )
	{
		if( r ) strcat( buf, "&" );
		else    strcat( buf, "?" );
		URL_Encode( reqs[ r ].param, param, BUFFERSIZE_ENCODE + 1 );
		strcat( buf, reqs[ r ].key );
		strcat( buf, "="           );
		strcat( buf, param         );
	}
	
	strcat( buf, " HTTP/1.1\r\n" );
	strcat( buf, "Host: "       ); strcat( buf, _host_name  ); strcat( buf, "\r\n" );
	strcat( buf, "User-Agent: " ); strcat( buf, _agent_name ); strcat( buf, "\r\n" );
	strcat( buf, "\r\n" );
}

static bool _FindPixelMark( TextReader *tr, bool b_found_ok )
{
	bool b_find = false;
	char line_buf[ BUFFERSIZE_MSGLINE ];

	if( !tr->toFind( "<html" ) ) goto End;
	if( !tr->toNextLine()      ) goto End;
	if( !tr->GetOneLine( line_buf, BUFFERSIZE_MSGLINE ) ) goto End;
	if( memcmp( line_buf, _pixel_mark, strlen( _pixel_mark ) ) ) goto End;
	if( b_found_ok )
	{
		if( !tr->GetOneLine( line_buf, BUFFERSIZE_MSGLINE ) ) goto End;
		if( memcmp( line_buf, _ok_mark, strlen( _ok_mark ) ) ) goto End;
	}
	b_find = true;
End:
	return b_find;
}

static const char* _Safe_strtok( char *str, const char *delimiter )
{
	const char *p = strtok( str, delimiter );
	static const char *_dum = "";
	if( !p ) return _dum;
	return p;
}



//--------
// Global
//--------

void CGI_Initialize( void ){ _buf = new _Buffer( BUFFERSIZE_USERFILE ); }
void CGI_Release   ( void ){ if( _buf ) delete _buf; }

bool CGI_GetServerInformation( SERVERINFORMATION *p_sv, const bool *p_b_stop )
{
	bool       b_ret   = false;
	wsClient   *client = new wsClient();
	TextReader *tr     = NULL;
	int        err;
	int        recv_size;

	memset( p_sv, 0, sizeof(SERVERINFORMATION) );

	dlog( "cgi get server info()" );

	if( !client->Connect( _host_name, _port, &err ) ){ dlog( "x cnct" ); goto End; }

	_MakeRequest( _buf->p, _cgi_server, 0, NULL );

	if(              !client->Send( _buf->p, strlen( _buf->p ),        p_b_stop )        ){ dlog( "x send" ); goto End; }
	if( ( recv_size = client->Recv( _buf->p, _buf->size       , false, p_b_stop ) ) <= 0 ){ dlog( "x recv" ); goto End; }

	if( recv_size > _buf->size - 1 ) recv_size = _buf->size - 1;
	_buf->p[ recv_size ] = '\0';

	if( !client->Shutdown() ){ dlog( "x shutdown" ); goto End; }

	tr = new TextReader( _buf->p, recv_size );

	if( !_FindPixelMark( tr, false ) ){ dlog( "x px mark ul" ); goto End; }
	else                                dlog( "o px mark ul" );

	char line_buf[ BUFFERSIZE_MSGLINE ];
	char buf     [ BUFFERSIZE_ENCODE  ];

	tr->GetOneLine( line_buf, BUFFERSIZE_MSGLINE );

	// date
	strcpy( buf, _Safe_strtok( line_buf, "\t" ) );

	// status
	strcpy( buf, _Safe_strtok( NULL    , "\t" ) );
	p_sv->status = stricmp( buf, "open" ) ? SERVERSTATUS_Close : SERVERSTATUS_Open;
		
	// ip address
	strcpy( p_sv->ip_address, _Safe_strtok( NULL, "\t" ) ); 

	// port no
	strcpy( buf, _Safe_strtok( NULL    , "\t" ) );
	p_sv->port_no = atoi( buf );

	// protocol
	strcpy( p_sv->protocol, _Safe_strtok( NULL    , "\t" ) );

	// quota
	strcpy( buf, _Safe_strtok( NULL    , "\t" ) );
	p_sv->version = atoi( buf );

	// comment
	for( int c = 0; c < SERVERCOMMENT_NUM; c++ )
	{
		strcpy( buf, _Safe_strtok( NULL, "\t" ) );
		if( !strcmp( buf, "<br>" ) ) memset(          p_sv->comments[ c ], 0, BUFFERSIZE_SERVERCOMMENT );
		else                         URL_Decode( buf, p_sv->comments[ c ],    BUFFERSIZE_SERVERCOMMENT );
	}

	b_ret = true;
End:
	if( client ) delete client;
	if( tr     ) delete tr    ;
	return b_ret;
}


bool CGI_SetServerInformation( const SERVERINFORMATION *p_sv, const bool *p_b_exit )
{
	wsClient   *client = new wsClient();
	TextReader *tr     = NULL;
	bool       b_ret   = false;
	int        err;

	dlog( "cgi set server info." );

	if( !client->Connect( _host_name, _port, &err ) ){ dlog( "x cnnct" ); goto End; }

	_REQUESTPARAMETER reqs       [ 11 ];
	char              str_port   [  8 ];
	char              str_version[  8 ];

	itoa( p_sv->port_no, str_port , 10 );
	itoa( p_sv->version, str_version, 10 );

	// buf
	reqs[ 0 ].key = "status"  ;
	if( p_sv->status == SERVERSTATUS_Open ) reqs[ 0 ].param = "open" ;
	else                                    reqs[ 0 ].param = "close";

	reqs[  1 ].key = "ip"      ; reqs[     1 ].param = p_sv->ip_address   ;
	reqs[  2 ].key = "port"    ; reqs[     2 ].param = str_port           ;
	reqs[  3 ].key = "protocol"; reqs[     3 ].param = p_sv->protocol     ;
	reqs[  4 ].key = "version" ; reqs[     4 ].param = str_version        ;
	reqs[  5 ].key = "comment0"; reqs[ 5 + 0 ].param = p_sv->comments[ 0 ];
	reqs[  6 ].key = "comment1"; reqs[ 5 + 1 ].param = p_sv->comments[ 1 ];
	reqs[  7 ].key = "comment2"; reqs[ 5 + 2 ].param = p_sv->comments[ 2 ];
	reqs[  8 ].key = "comment3"; reqs[ 5 + 3 ].param = p_sv->comments[ 3 ];
	reqs[  9 ].key = "comment4"; reqs[ 5 + 4 ].param = p_sv->comments[ 4 ];
	reqs[ 10 ].key = "comment5"; reqs[ 5 + 5 ].param = p_sv->comments[ 5 ];

	_MakeRequest( _buf->p, _cgi_server, 11, reqs );

	if( !client->Send( _buf->p, strlen( _buf->p ), p_b_exit ) ){ dlog( "x send" ); goto End; }

	int recv_size = client->Recv( _buf->p, _buf->size, false, p_b_exit  );
	if( recv_size <= 0      ){ dlog( "x recv" ); goto End; }
	if( recv_size > _buf->size - 1 ) recv_size = _buf->size - 1;
	_buf->p[ recv_size ] = '\0';

	if( !client->Shutdown() ){ dlog( "x shutdown" ); goto End; }

	tr = new TextReader( _buf->p, recv_size );

	if( !_FindPixelMark( tr, false ) ){ dlog( "x px mark w" ); goto End; }
	else                                dlog( "o px mark w" );

	b_ret = true;
End:

	if( client ) delete client;
	if( tr     ) delete tr    ;

	return b_ret;
}
