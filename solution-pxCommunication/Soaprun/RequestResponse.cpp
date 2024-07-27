#include <StdAfx.h>

#include "../Fixture/RingBuffer.h"
#include "../Fixture/DebugLog.h"

#include "RequestResponse.h"



static char _buf[ 128 ] = "";

extern RingBuffer     *g_ring_req ;

void Request_Dlog( const char *fmt, ... )
{
	reqDLOG *p_req_dlog = (reqDLOG*)_buf;

	va_list ap;
	va_start( ap, fmt );
	vsprintf( &_buf[ sizeof(reqDLOG) ], fmt, ap );
	va_end( ap );

	int len = strlen( (char*)&_buf[ sizeof(reqDLOG) ] );

	memcpy ( p_req_dlog->req_name, g_req_names[ clREQUEST_Dlog ], BUFFERSIZE_REQNAME );
	p_req_dlog->str_len = len + 1;
	g_ring_req->bin_Push( &_buf, sizeof(reqDLOG) + len + 1 );
}

void Request_mCorpse( short x, short y )
{
	reqMAPCORPSE *p_req = (reqMAPCORPSE*)_buf;
	memcpy( p_req->req_name, g_req_names[ clREQUEST_mCorpse ], BUFFERSIZE_REQNAME );
	p_req->x     = x;
	p_req->y     = y;
	if( !g_ring_req->bin_Push( _buf, sizeof(reqMAPCORPSE) ) ) dlog( "can't req push: mCrps!" );
}

void Request_Bye()
{
	reqBYE *p_req = (reqBYE*)_buf;
	memcpy( p_req->req_name, g_req_names[ clREQUEST_Bye ], BUFFERSIZE_REQNAME );
	if( !g_ring_req->bin_Push( _buf, sizeof(reqBYE) ) ) dlog( "can't req push: Bye!" );
}
