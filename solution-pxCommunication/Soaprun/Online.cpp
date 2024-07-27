#include <StdAfx.h>

#include "../Fixture/RingBuffer.h"
#include "../Fixture/DebugLog.h"

#include "RequestResponse.h"
#include "Attender.h"

extern RingBuffer *g_ring_msg;
extern RingBuffer *g_ring_req;
extern RingBuffer *g_ring_res;

static HANDLE     _h_thrd               = NULL ;
static HANDLE     _hEve_StartConnection = NULL ;
static HANDLE     _hEve_Exit            = NULL ;
static bool       _b_exit               = false;
static bool       _b_wait_event         = false;
static bool       _b_online             = false;

static Attender   *_attender = NULL;

static unsigned __stdcall _Thread_Attender( void *p )
{
	HANDLE handles[ 2 ];
	DWORD  res;

	while( !_b_exit )
	{
		// 接続GOサインを待つ
		handles[ 0 ] = _hEve_StartConnection;
		handles[ 1 ] = g_hEve_Exit          ;

		_b_wait_event = true ;
		res = WaitForMultipleObjects( 2, handles, FALSE, INFINITE );

		_b_wait_event = false;

		if( res != WAIT_OBJECT_0 ){ dlog( "wait go connection: false." ); break; }
		if( _b_exit ) break;

		g_ring_msg->txt_Push( "せつぞく かいし…" );

#ifdef NDEBUG
		if( _attender->GetServerInformation() ) 
#else
		_attender->SetServer( "192.168.0.157", 1002 );
#endif
		{
			if( _b_exit ) break;

			if( _attender->Connection() )
			{
				g_ring_msg->txt_Push( "せつぞくしました。"  );
				if( _attender->CheckQuota( &_b_exit ) && _attender->CheckProtocol( &_b_exit ) )
				{
					g_ring_msg->txt_Push( "プロトコルかくにんOK."  );
					if( _attender->SpeedTest( &_b_exit ) )
					{
						g_ring_msg->txt_Push( "つうしん あんてい。"  );
						_b_online = true ;
						_attender->RequestAndResponse( g_ring_req, g_ring_res, &_b_exit );
					}
				}
				_attender->ShutdownSocket();
				_b_online = false;
			}
		}
	}

	SetEvent( _hEve_Exit );
	_h_thrd = NULL;
	_endthreadex( 0 );
	return 0;
}

bool Online_Initialize()
{
	unsigned int id;

	_attender = new Attender();

	if( !( _hEve_StartConnection = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) return false;
	if( !( _hEve_Exit            = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) return false;

	if( 0 )
	{
		_Thread_Attender( NULL );
	}
	else
	{
		if( !( _h_thrd = (HANDLE)_beginthreadex( NULL, 0, _Thread_Attender, (LPVOID)&id, 0, &id ) ) ) return false;
	}

	return true;
}

void Online_StartConnection()
{
	_attender->Reset();
    SetEvent( _hEve_StartConnection );
}

bool Online_IsConnection()
{
	return _b_online;
}

bool Online_IsWaitEvent()
{
	return _b_wait_event;
}

void Online_Shutdown()
{
	_attender->ClientShutdown();
}

bool Online_WaitExit( int msec_timeout )
{
	bool b_ret = false;

	_b_exit = true;

	if( WaitForSingleObject( _hEve_Exit, msec_timeout ) == WAIT_OBJECT_0 ) b_ret = true;
	CloseHandle( _hEve_StartConnection );
	CloseHandle( _hEve_Exit            );
	if( _attender ) delete _attender;

	return b_ret;
}
