#include <limits.h>     // PATH_MAX / NAME_MAX

#ifdef NDEBUG

#include <windows.h>
static void _sleep_msec( int msec ){ Sleep( msec         ); }

#else

#include <pthread.h>
#include <unistd.h>     // usleep()
static void _sleep_msec( int msec ){ usleep( msec * 1000 ); }

#endif

#include <stdio.h>      // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <stdlib.h>     // mode_t / exit() / malloc()
#include <string.h>     // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <errno.h>      // errno

#include "SharedDefines.h"

#include "DebugLog.h"
#include "LinkUnits.h"
#include "lnxSock.h"
#include "RequestResponse.h"
#include "Signal.h"
#include "svFieldUnits.h"
#include "svAttender.h"

extern LinkUnits    *g_link_units   ;
extern const char   *g_protocol_name;
extern dt_MapData   *g_mAttr        ;
extern dt_MapData   *g_map          ;
extern svFieldUnits *g_funits       ;



static void *_Thread( void *arg )
{
	bool           b_ret = false           ;
	svAttender     *atnd = (svAttender*)arg;
	const LINKUNIT *p_link;

#ifdef NDEBUG
	goto End;
#else
	if( !Signal_Mask( NULL ) ) goto End;
#endif

//	printf( "a[%2d] on.\n", atnd->Index() );

	while( !g_exit )
	{
		if( p_link = g_link_units->GetLink( atnd->Index() ) )
		{
			atnd->LinkUnit_Set( p_link );
			if( atnd->SendQuota() )
			{
				while( !g_exit && atnd->RecvRequiestAndSendResponse() ){}
			}
			atnd->LinkUnit_Close();
		}
		_sleep_msec( 1000 );
	}

	b_ret = true;
End:
	
	return arg;
}


svAttender::svAttender( int index, bool b_over_quota )
{
	_b_ok         = false         ;
	_status       = _ATTENDER_IDLE;
	_thrd         =              0;
	_index        = index         ;
	_buf          = NULL          ;
	_b_over_quota = b_over_quota  ;
	_p_link       = NULL          ;
	_gene_count   =              0;

	if( !( _buf = (char*)malloc( BUFFERSIZE_ATTENDER ) ) ) return;
	_buf_size  = BUFFERSIZE_ATTENDER;
	_mbuf      = &_buf[                sizeof(int) ];
	_mbuf_size = BUFFERSIZE_ATTENDER - sizeof(int)  ;

	_room_send = new dt_MapData( ROOMGRIDNUM_H + 1, ROOMGRIDNUM_V + 1    );
	_room_send->SetSize        ( ROOMGRIDNUM_H + 1, ROOMGRIDNUM_V + 1, 0 );

#ifdef NDEBUG
#else
	int  th_stat;
	th_stat   = pthread_create( &_thrd, NULL, _Thread, (void *)this );
	if( th_stat ){ dlog( "thrd attender: ERR." ); _status = _ATTENDER_ERROR; return; }
	else         { dlog( "thrd attender: OK."  ); }
#endif

	_b_ok = true;
}

svAttender::~svAttender()
{
	if( _room_send ) delete _room_send;
//	dlog( "sv attender" );
	if( _buf       ) free( _buf )     ;
}

bool svAttender::WaitExit()
{
	_b_stop = true;

#ifdef NDEBUG
#else
	void *th_res;
	pthread_join( _thrd, &th_res );
#endif

	return true;
}

int svAttender::Index()
{
	return _index;
}

void svAttender::LinkUnit_Set( const LINKUNIT *p_link )
{
	_p_link         = p_link;
	_last_update_id =      0;
	
	g_funits->StartPlayer( _index );

	dlog( "a[%2d] sock=%d id=%s ip=%s", _index, p_link->sock, p_link->host_id, p_link->host_address );
}

void svAttender::LinkUnit_Close()
{
	g_funits->RemovePlayer( _index );
	if( !g_link_units->RemoveBeforeClose( _index, _p_link ) ) return;
	g_link_units->ShutdownAndClose      ( _index, _p_link );
	dlog( "a[%2d]: close.", _index );
	return;
}

int  svAttender::_Response_Protocol()
{
	resPROTOCOL *p_res = (resPROTOCOL*)_mbuf;
	memcpy( p_res->res_name, g_res_names[ svRESPONSE_Protocol ], BUFFERSIZE_RESNAME  );
	memcpy( p_res->protocol, g_protocol_name                  , BUFFERSIZE_PROTOCOL );
	p_res->version = g_protocol_version;
	return lnxSock_Send_mbuf( _mbuf, sizeof(resPROTOCOL), _p_link->sock, &_b_stop );
}

int  svAttender::_Response_MapAttribute()
{
	resMAPATTRIBUTE *p_res = (resMAPATTRIBUTE*)_mbuf;
	short           w      = (short)g_mAttr->get_w();
	short           l      = (short)g_mAttr->get_l();
	memcpy( p_res->res_name, g_res_names[ svRESPONSE_MapAttribute ], BUFFERSIZE_RESNAME  );
	p_res->w = w;
	p_res->l = l;
	memcpy(  &_mbuf[ sizeof(resMAPATTRIBUTE) ], g_mAttr->get_p(), w * l );
	return lnxSock_Send_mbuf( _mbuf, sizeof(resMAPATTRIBUTE) + w * l, _p_link->sock, &_b_stop );
}

int  svAttender::_Response_Room( char x, char y )
{
	short   w      = (short)_room_send->get_w();
	short   l      = (short)_room_send->get_l();
	resROOM *p_res = (resROOM*)_mbuf;

	if( !g_map->GetClip( _room_send, x * ROOMGRIDNUM_H, y * ROOMGRIDNUM_V ) )
	{
		_room_send->Fill( 1 );
	}

	memcpy( p_res->res_name, g_res_names[ svRESPONSE_Room ], BUFFERSIZE_RESNAME  );
	p_res->x = x;
	p_res->y = y;
	memcpy( &_mbuf[ sizeof(resROOM) ], _room_send->get_p(), w * l );
	return lnxSock_Send_mbuf( _mbuf, sizeof(resROOM) + w * l, _p_link->sock, &_b_stop );
}

int  svAttender::_Response_Test()
{
	memcpy( _mbuf, g_res_names[ svRESPONSE_Test ], BUFFERSIZE_RESNAME );
	return lnxSock_Send_mbuf( _mbuf, BUFFERSIZE_TEST, _p_link->sock, &_b_stop );
}

int  svAttender::_Response_Void()
{
	memcpy( _mbuf, g_res_names[ svRESPONSE_Void ], BUFFERSIZE_RESNAME );
	return lnxSock_Send_mbuf( _mbuf, sizeof(resVOID), _p_link->sock, &_b_stop );
}

int  svAttender::SendQuota()
{
	memcpy( _mbuf    ,         _b_over_quota ? "OVER"       : "WLCM"   , 4 );
	dlog( "a[%2d] %s", _index, _b_over_quota ? "over quota" : "welcome"    );
	if( lnxSock_Send_mbuf( _mbuf, 4, _p_link->sock, &_b_stop ) != 4 ) return false;
	if(   !_b_over_quota ) _gene_count++;
	return _b_over_quota ? false : true;
}

int  svAttender::_Response_FieldUnits()
{
	int data_size = g_funits->GetUnits( _mbuf, _mbuf_size, _index, &_last_update_id );
	if( data_size < 0 ){ dlog( "a[%2d]get units: buffer full!", _index ); return -1; }
	memcpy( _mbuf, g_res_names[ svRESPONSE_Field ], BUFFERSIZE_RESNAME );
	return lnxSock_Send_mbuf( _mbuf, data_size, _p_link->sock, &_b_stop );
}

bool svAttender::RecvRequiestAndSendResponse()
{
	int size = lnxSock_Recv( _buf, _buf_size, _p_link->sock, true, &_b_stop );
	if( size <= 0                  ){ dlog( "a[%2d] recv: size zero!", _index ); return false; }
	if( size <  BUFFERSIZE_REQNAME ){ dlog( "a[%2d] recv: size low!" , _index ); return false; }

	int r;

	for( r = 1; r < clREQUEST_num; r++ )
	{
		if( !memcmp( _buf, g_req_names[ r ], BUFFERSIZE_REQNAME ) ) break;
	}

	reqPROTOCOL  *p_req_prtc       ;
	reqROOM      *p_req_room       ;
	reqDLOG      *p_req_dlog       ;
	reqMAPCORPSE *p_req_mcrp       ;

	unsigned char mparts;

	switch( r )
	{
	case clREQUEST_Protocol    : p_req_prtc = (reqPROTOCOL*)_buf;
								 dlog( "a[%2d] cl v.%04d", _index, p_req_prtc->version );
								 if( _Response_Protocol     () <= 0 ) return false; break;

	case clREQUEST_Test        : if( _Response_Test         () <= 0 ) return false; break;

	case clREQUEST_MapAttribute: if( _Response_MapAttribute () <= 0 ) return false; break;

	case clREQUEST_Room        : p_req_room = (reqROOM*)_buf;
//								 printf( "a[%2d] room(%d,%d)\n", _index, p_req_room->x, p_req_room->y );
								 if( _Response_Room( p_req_room->x, p_req_room->y ) <= 0 ) return false;
								 break;


	case clREQUEST_mCorpse     : p_req_mcrp = (reqMAPCORPSE*)_buf;
								 dlog( "a[%2d] mCrps %d %d", _index,  p_req_mcrp->x, p_req_mcrp->y );
								 mparts = g_map->SetCorpse (          p_req_mcrp->x, p_req_mcrp->y, true  );
								 if( mparts ) g_funits->PushMapParts( p_req_mcrp->x, p_req_mcrp->y, mparts );
								 if( _Response_Void() <= 0 ) return false; break;
								 break;

	case clREQUEST_Dlog        : p_req_dlog = (reqDLOG*)_buf;
								 dlog( "a[%2d] %s", _index, (char*)&_buf[ sizeof(reqDLOG) ] );
								 if( _Response_Void() <= 0 ) return false;
								 break;

	case clREQUEST_Bye         : if( _Response_Void() > 0 ) dlog( "a[%2d] Bye.", _index ); return false;
								 break;

	case clREQUEST_INQ_HitNPU     :
	case clREQUEST_INQ_HitVenger  :
	case clREQUEST_INQ_ChangeColor:
	case clREQUEST_INQ_DrawFloor  :
	case clREQUEST_MyPosition     :

		switch( r )
		{
		case clREQUEST_MyPosition  :
			{
				reqMYPOSITION *p_req = (reqMYPOSITION*)_buf;
				if( !g_funits->SetPlayerPosition( _index, _gene_count, &p_req->pos_his_num ) ) return false;
			}
			break;

		case clREQUEST_INQ_HitNPU:
			{
				reqINQ_HitNPU *p_req = (reqINQ_HitNPU*)_buf;
				if( !g_funits->SetPlayerPosition( _index, _gene_count, &p_req->pos_his_num ) ) return false;
				g_funits->Inquiry_HITNPU( _index, _gene_count, p_req->uc_tgt );
			}
			break;

		case clREQUEST_INQ_HitVenger:
			{
				reqINQ_HitVenger *p_req = (reqINQ_HitVenger*)_buf;
				if( !g_funits->SetPlayerPosition( _index, _gene_count, &p_req->pos_his_num ) ) return false;
				g_funits->Inquiry_HitVenger( _index );
			}
			break;

		case clREQUEST_INQ_ChangeColor:
			{
				reqINQ_ChangeColor *p_req = (reqINQ_ChangeColor*)_buf;
				if( !g_funits->SetPlayerPosition( _index, _gene_count, &p_req->pos_his_num ) ) return false;
				g_funits->Inquiry_ChangeColor( _index, p_req->param1 );
			}
			break;

		case clREQUEST_INQ_DrawFloor   :
			{
				reqINQ_DrawFloor *p_req = (reqINQ_DrawFloor*)_buf;
				if( g_map->SetMapParts( p_req->fld_x, p_req->fld_y, p_req->i ) )
				{
					g_funits->NPUGrid_Update( p_req->fld_x, p_req->fld_y );
					g_funits->PushMapParts( p_req->fld_x, p_req->fld_y, p_req->i );
				}
			}
		}

		if( _Response_FieldUnits() <= 0 ) return false;
		break;

	default             : dlog( "a[%2d] recv: unknown! %d", _index, *_buf ); return false;
	}

	return true;
}

