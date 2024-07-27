#include <StdAfx.h>

#include "../Fixture/RingBuffer.h"
#include "../Fixture/DebugLog.h"
#include "../SoapCGI/CGI.h"

#include "RequestResponse.h"
#include "Attender.h"

extern RingBuffer     *g_ring_msg;
extern int            g_version  ;


Attender::Attender()
{
	_cl   = new wsClient();
	_buf  = (char*)malloc(  BUFFERSIZE_ATTENDER  );
	_mbuf = &_buf[ sizeof(int) ];
	Reset();
}
Attender::~Attender()
{
	if( _buf ) free( _buf );
	if( _cl  ) delete _cl  ;
}

void Attender::Reset()
{
	memset( _buf      , 0, BUFFERSIZE_ATTENDER  );
	memset( _server_ip, 0, BUFFERSIZE_IPADDRESS );
	_port_no       =     0;
	_b_cl_shutdown = false;
}

bool Attender::GetServerInformation()
{
	bool              b_ret = false;
	SERVERINFORMATION sv           ;

	if( !CGI_GetServerInformation( &sv, &_b_cl_shutdown ) )
	{
		g_ring_msg->txt_Push(
			"サーバーしゅとくエラー\n\n"
			"げんいん は\n" "オフライン か\n" "サービスていし か\n"
			"サービスしゅうりょう。\n" "もしくは バグ。" );

		goto End;
	}

	if( sv.status != SERVERSTATUS_Open )
	{
		g_ring_msg->txt_Push( "エラー\nサーバーは ねている。\n" );
		for( int c = 0; c < SERVERCOMMENT_NUM; c++ )
		{
			if( strlen( sv.comments[ c ] ) ) g_ring_msg->txt_Push( sv.comments[ c ] );
		}
		goto End;
	}

	if( memcmp( sv.protocol,  g_protocol_name, BUFFERSIZE_PROTOCOL ) ){ g_ring_msg->txt_Push( "エラー\nサーバーちがい。"     ); goto End; }
	if(         sv.version != g_protocol_version                     ){ g_ring_msg->txt_Push( "エラー\nバージョンがふるい。" ); goto End; }

	g_ring_msg->txt_Push( "サーバーじょうほうOK."      );

	b_ret = true;
	strcpy( _server_ip, sv.ip_address );
	_port_no = sv.port_no;
End:

	return b_ret;
}

void Attender::SetServer( const char*ip, int port_no )
{
	strcpy( _server_ip, ip );
	_port_no = port_no;
}

bool Attender::Connection()
{
	bool b_ret = false;
	int  err   =     0;

	dlog( "connect: %s-%d", _server_ip, _port_no );
	if( !_cl->Connect( _server_ip, _port_no, &err ) ){ dlog( "x cnct: %d", err ); goto End; }
	dlog( "connect: OK." );
	b_ret = true;
End:
	if( !b_ret )
	{
		g_ring_msg->txt_Push( 
			"でも、せつぞくできず…\n\n"
			"げんいん は\n"
			"ていいんオーバー か\n"
			"サーバーねている か\n"
			"サービスしんだ。\n"
			"もしくは バグ。" );    
	}
	return b_ret;
}

bool Attender::CheckQuota( const bool *p_b_stop )
{
	bool b_ret = false;
	int  recv_size    ;

	recv_size = _cl->Recv( _buf, BUFFERSIZE_ATTENDER, true, p_b_stop );
	if( recv_size != 4 ){ dlog( "lack quota: %d byte", recv_size ); goto End; }
	if( memcmp( _buf, "WLCM", 4 ) ) goto End;

	b_ret         = true;
End:
	if( !b_ret ) g_ring_msg->txt_Push( "ていいんオーバーです…\nそれか バグ。" );
	return b_ret;
}


bool Attender::CheckProtocol( const bool *p_b_stop )
{
	bool        b_ret  = false;
	reqPROTOCOL *p_req = (reqPROTOCOL*)_mbuf;
	resPROTOCOL *p_res = (resPROTOCOL*)_buf ;
	int         recv_size     ;

	dlog( "Protocol: %s %04d.", g_protocol_name, g_protocol_version );

	memcpy( p_req->req_name, g_req_names[ clREQUEST_Protocol ], BUFFERSIZE_REQNAME );
	p_req->version = (short)g_version;

	if( !_cl->Send_mbuf( _mbuf, sizeof(reqPROTOCOL), p_b_stop ) ){ dlog( "x send Prtc" ); goto End; }
	recv_size = _cl->Recv( _buf, BUFFERSIZE_ATTENDER, true, p_b_stop );
	if( recv_size != sizeof(resPROTOCOL) ){ dlog( "lack Protocol: %d byte", recv_size ); goto End; }

	if( memcmp( p_res->res_name, g_res_names[ svRESPONSE_Protocol ], BUFFERSIZE_RESNAME  ) ){ dlog( "x res-name"                   ); goto End; }
	if( memcmp( p_res->protocol,  g_protocol_name                  , BUFFERSIZE_PROTOCOL ) ){ dlog( "x protocol"                   ); goto End; }
	if(         p_res->version != g_protocol_version                                       ){ dlog( "x proto v %d", p_res->version ); goto End; }

	b_ret         = true;
End:

	if( !b_ret ) g_ring_msg->txt_Push( "プロトコルエラー\nバージョンふるい。\nもしくは バグ。" );
	return b_ret;
}


bool Attender::SpeedTest( const bool *p_b_stop )
{
	bool b_ret = false;
	int  recv_size    ;
	char *p = &_buf[ sizeof(int) ];
	DWORD ress[ 4 ];

	for( int i = 0; i < 4; i++ )
	{
		ress[ i ] = GetTickCount();
		*(int*)_buf = BUFFERSIZE_TEST;
		strcpy( p, g_req_names[ clREQUEST_Test ] );
		if( !_cl->Send( _buf, BUFFERSIZE_TEST + sizeof(int), p_b_stop ) ){ dlog( "x send: test" ); goto End; }

		recv_size = _cl->Recv( _buf, BUFFERSIZE_ATTENDER, true, p_b_stop );
		if( recv_size != BUFFERSIZE_TEST ){ dlog( "lack test: %d byte", recv_size ); goto End; }
		if( memcmp( _buf, g_res_names[ svRESPONSE_Test ], BUFFERSIZE_RESNAME  ) ){ dlog( "x res-name" ); goto End; }
		ress[ i ] = GetTickCount() - ress[ i ];
	}

	Request_Dlog( "%dbyte %d,%d,%d,%dmsec", BUFFERSIZE_TEST, ress[ 0 ], ress[ 1 ], ress[ 2 ], ress[ 3 ] );
	b_ret = true;
End:
	if( !b_ret )
	{
		g_ring_msg->txt_Push( "つうしん が ふあんてい。" );
	}
	return b_ret;

}

void Attender::RequestAndResponse( RingBuffer *_ring_req, RingBuffer *_ring_res, const bool *p_b_exit )
{
	// *p_b_exit      アプリ終了
	//  b_sv_shutdown サーバーからの切断
	// _b_cl_shutdown クライアントからの切断

	int  recv_size;
	int  data_size;
	bool b_sv_shutdown = false;
	char *p = &_buf[ sizeof(int) ];

//	_ring_req ->Clear();
//	_ring_res ->Clear();

	while( !*p_b_exit && !b_sv_shutdown && !_b_cl_shutdown )
	{
		if( _ring_req->IsPop() )
		{
			data_size = _ring_req->bin_Pop( _mbuf );
			if( _cl->Send_mbuf( _mbuf, data_size, &_b_cl_shutdown ) )
			{
				recv_size = _cl->Recv( _buf, BUFFERSIZE_ATTENDER, true, &_b_cl_shutdown );
				if( recv_size > 0 ) _ring_res->bin_Push( _buf, recv_size );
				else                b_sv_shutdown = true;
			}
			else b_sv_shutdown = true;
		}
		Sleep( 100 );
	}

	if( b_sv_shutdown ) g_ring_msg->txt_Push( "つうしんエラー" );
}

void Attender::ClientShutdown()
{
	_b_cl_shutdown = true;
}

void Attender::ShutdownSocket()
{
	_cl->Shutdown();
}
