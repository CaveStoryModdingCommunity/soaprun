#include <StdAfx.h>

/*
#include "../Fixture/RingBuffer.h"
#include "../Fixture/DebugLog.h"

#include "SharedDefines.h"
#include "RequestResponse.h"
#include "Inquiry.h"

extern RingBuffer *g_ring_req;

bool Inquiry::_Request( INQTYPE type )
{
	reqINQUIRY req;

	memcpy( req.req_name, g_req_names[ clREQUEST_Inqry ], BUFFERSIZE_REQNAME );
	req.c_type  = (char)type;
	req.uc_tgt  = _uc_tgt    ;
	req.s_x     = _fld_x     ;
	req.s_y     = _fld_y     ;

	if( !g_ring_req->bin_Push( &req, sizeof(reqINQUIRY) ) )
	{
		dlog( "can't req push: Inq!" ); return false;
	}
	_b_lock = true;
	return true;
}

Inquiry::Inquiry()
{
	_b_lock = false;
	_uc_tgt =     0;
//	_count  =     0;
}

void Inquiry::Unlock()
{
	_b_lock = false;
//	_count++;
}

bool Inquiry::req_HitNPU( int tgt_index )
{
	if( _b_lock ) return false;
	_uc_tgt = tgt_index ;
//	_count++;
	return _Request( INQ_HitNPU );
}

bool Inquiry::req_HitVenger( int tgt_index )
{
	if( _b_lock ) return false;
	_uc_tgt = tgt_index    ;
//	_count++;
	return _Request( INQ_HitVenger );
}

bool Inquiry::req_DrawFloor( short fld_x, short fld_y, unsigned char draw_index )
{
	if( _b_lock ) return false;
	_fld_x      = fld_x;
	_fld_y      = fld_y;
	_draw_index = draw_index;
//	_count++;
	return _Request( INQ_DrawFloor );
}

bool Inquiry::req_ChangeColor( char color )
{
	if( _b_lock ) return false;
	_uc_tgt = color;
//	_count++;
	return _Request( INQ_ChangeColor );
}
/*
bool Inquiry::CheckCount( char count )
{
	if( count == _count ) return true;
	return false;
}
*/
