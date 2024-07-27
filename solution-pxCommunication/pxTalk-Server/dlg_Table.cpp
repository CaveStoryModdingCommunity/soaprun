#include <StdAfx.h>

#include "../ws/wsServer.h"

#include "../Fixture/WindowRect.h"
#include "../Fixture/WindowFlash.h"
#include "../Fixture/RingBuffer.h"
#include "../Fixture/StringBuffer.h"
#include "../Fixture/DebugLog.h"
#include "../Fixture/ShiftJIS.h"
#include "../Fixture/EditBox.h"

#include "resource.h"

#include "PortNo.h"


#define _TIMERWAIT        100
#define _THREAD_INTERVAL  100

#define _RINGBUFFERSIZE  1024


static const char *_rect_name  = "server-table.rect";

static HANDLE      _h_thrd      = NULL ;
static int         _timer_id    =   106;
static bool        _b_connected = false;
static bool        _b_stop      = false;
static RingBuffer *_ring_msg   = NULL ;

static void _Timer_Kill( HWND hDlg ){ if( !KillTimer( hDlg, _timer_id ) ){} }
static void _Timer_Set ( HWND hDlg ){ _timer_id = SetTimer( hDlg, _timer_id, _TIMERWAIT, NULL ); }
static void _IDC_Enable( HWND hDlg, int idc, bool b ){ EnableWindow( GetDlgItem( hDlg, idc ), b ? TRUE : FALSE ); }

static CRITICAL_SECTION _cs_talk_window;

enum _NFUNC
{
	_NFUNC_Ready = 0,
	_NFUNC_Idle,
	_NFUNC_Text,
};

typedef struct
{
	_NFUNC func;
	char   text[ BUFFERSIZE_TEXT + 1 ];
}
_MESSAGEORDERSTRUCT;

static _MESSAGEORDERSTRUCT _order;

enum _LASTMESSAGE
{
	_LAST_MSG_NONE = 0,
	_LAST_MSG_OWN     ,
	_LAST_MSG_OTHER   ,
};

static _LASTMESSAGE _last_msg = _LAST_MSG_NONE;


static void _EnableSendControls( HWND hDlg, bool b )
{
	_IDC_Enable( hDlg, IDC_BTN_SEND  , b );
}




#define _TEST_BUFFERSIZE 512
#define _TEST_MAX_COUNT  (60*5)
#define _TEST_SLEEP      1
#define _TEST_VERSION    12220058

typedef struct
{
	int recv_code;
	int recv_tick;
	int send_code;
	int send_tick;
}
_TESTRECORD;

static bool _Test( wsServer *p_server )
{
	_ring_msg->txt_Push( "テスト中…" );

	char    bf  [  _TEST_BUFFERSIZE ];
	memset( bf, 0, _TEST_BUFFERSIZE );

	_TESTRECORD recs[ _TEST_MAX_COUNT ];
	DWORD       tick, t;
	int         recv_size;

	memset( recs, 0, sizeof(recs) );

	tick = GetTickCount();
	for( int i = 0; !_b_stop && i < _TEST_MAX_COUNT; i++ )
	{
		// Receive..
		recv_size = p_server->Recv( bf, _TEST_BUFFERSIZE, true, &_b_stop );
		t    = GetTickCount();
		recs[ i ].recv_tick = t - tick;
		tick = t;

		if( recv_size < 0 ){ _ring_msg->txt_Push( "Recv: ERR.[ %d ]", i ); return false; }
		if( recv_size == _TEST_BUFFERSIZE )
		{
			memcpy( &recs[ i ].recv_code, &bf[ _TEST_BUFFERSIZE - 4 ], sizeof(int) );
		}
		else
		{
			recs[ i ].recv_code = -1;
		}

		// Send..
		recs[ i ].send_code = i;
		memcpy( &bf[ _TEST_BUFFERSIZE - 4 ], &recs[ i ].send_code, sizeof(int) );
		if( !p_server->Send( bf, _TEST_BUFFERSIZE, true, &_b_stop ) ){ _ring_msg->txt_Push( "Send: ERR.[ %d ]", i ); return false; }
		t    = GetTickCount();
		recs[ i ].send_tick = t - tick;
		tick = t;

		if( _TEST_SLEEP ) Sleep( _TEST_SLEEP );
	}

	char path_dst[ MAX_PATH ];
	sprintf( path_dst, "%s\\%s", g_dir_profile, "test-server.txt" );
	FILE *fp = fopen( path_dst, "wt" );

	if( fp )
	{
		_ring_msg->txt_Push( "テスト結果を出力中." );

		fprintf( fp, "version %d, count %d, buf_size %d, sleep %d\n", _TEST_VERSION, _TEST_MAX_COUNT, _TEST_BUFFERSIZE, _TEST_SLEEP );
		
		fprintf( fp, "<Receives>\n" );
		for( int i = 0; !_b_stop && i < _TEST_MAX_COUNT; i++ )
		{
			fprintf( fp, "%04d,%04d\n", recs[ i ].recv_code, recs[ i ].recv_tick );
		}

		fprintf( fp, "\n" );

		fprintf( fp, "<Sends>\n" );
		for( int i = 0; !_b_stop && i < _TEST_MAX_COUNT; i++ )
		{
			fprintf( fp, "%04d,%04d\n", recs[ i ].send_code, recs[ i ].send_tick );
		}

		fclose( fp );
	}

	_ring_msg->txt_Push( "テスト完了." );

	return true;
}


static unsigned __stdcall _Thread_Table_Server( void *p )
{
	wsServer *p_server = (wsServer*)p;

	int      recv_size;

	PortNo   port;

	_ring_msg->txt_Push( "Bind and listen.." );
	if( !p_server->BindAndListen( port.port_no ) ){ _ring_msg->txt_Push( "Listen: Error." ); goto End; }
	_ring_msg->txt_Push( "Listen: OK." );

	_ring_msg->txt_Push( "Accepting.." );
	if( !p_server->Accept       (              ) ){ _ring_msg->txt_Push( "Accpt: Error."  ); goto End; }
	_ring_msg->txt_Push( "Accpt[ %s ]: OK.", p_server->ClientName() );

	_b_connected = true;

//	if( !_Test( p_server ) ) goto End;

	_order.func = _NFUNC_Idle;


	char buf[ BUFFERSIZE_TEXT ];

	while( !_b_stop )
	{
		recv_size = p_server->Recv( buf, BUFFERSIZE_TEXT, true, &_b_stop );
		if( recv_size < 0 ){ _ring_msg->txt_Push( "recv: ERR." ); goto End; }
		if( recv_size > 0 )  _ring_msg->txt_Push( buf );

		switch( _order.func )
		{
		case _NFUNC_Text:
			if( !p_server->Send( _order.text, strlen( _order.text ) + 1, true, &_b_stop ) )
			{
				_ring_msg->txt_Push( "send: ERR." );
				goto End;
			}
			_order.func = _NFUNC_Idle;
			break;

		default:
			if( !p_server->Send( "", 0, true, &_b_stop ) ){ _ring_msg->txt_Push( "send: ERR." ); goto End; }
			break;
		}

		Sleep( _THREAD_INTERVAL );
	}


End:

	while( !_b_stop ){ Sleep( _THREAD_INTERVAL ); }

	_h_thrd = NULL;
	_endthreadex( 0 );
	return 0;
}


static void _AddTalkWindow( HWND hDlg, const char *name, const char *text, _LASTMESSAGE last )
{
	EnterCriticalSection( &_cs_talk_window );

	if( !_b_stop )
	{
		char str[ BUFFERSIZE_NAME + BUFFERSIZE_TEXT + 2 ];

		if( last != _LAST_MSG_NONE )
		{
			bool b_name = false;
			if( _last_msg != last ) b_name = true;
			sprintf( str, "%s> %s", b_name ? name : "", text );
		}
		else
		{
			sprintf( str, "%s", text );
		}
		_last_msg = last;

		EditBox_AppendString( hDlg, IDC_TALKWINDOW, str );
	}

	LeaveCriticalSection( &_cs_talk_window );
}

static void _WM_TIMER( HWND hDlg, wsServer *p_server  )
{
	_Timer_Kill( hDlg );

	if( !_h_thrd )
	{
		dlog( "delete ring" ); if( _ring_msg ) delete _ring_msg; dlog( "delete ring: ok" );
		EndDialog( hDlg, TRUE );
		return;
	}

	if( _ring_msg->IsPop() )
	{
		char text[ BUFFERSIZE_TEXT + 1 ];
		while( _ring_msg->txt_Pop( text ) ) _AddTalkWindow( hDlg, "client", text, _b_connected ? _LAST_MSG_OTHER : _LAST_MSG_NONE );

		WindowFlash( hDlg );
	}

	if( _order.func == _NFUNC_Idle ) _EnableSendControls( hDlg, true );

	_Timer_Set( hDlg );
}

static void _IDC_BTN_SEND( HWND hDlg, wsServer *p_server )
{
	if( _order.func != _NFUNC_Idle ) return;

	char text[ BUFFERSIZE_TEXT + 1 ];

	GetDlgItemText( hDlg, IDC_MESSAGE, text, BUFFERSIZE_TEXT + 1 );
	SetDlgItemText( hDlg, IDC_MESSAGE, "" );

	_EnableSendControls( hDlg, false );

	_AddTalkWindow( hDlg, "client", text, _LAST_MSG_OWN );

	strcpy( _order.text, text );
	_order.func = _NFUNC_Text;
}



static wsServer *_p_server = NULL;

static void _WM_INITDIALOG( HWND hDlg, LPARAM l )
{
	_p_server = (wsServer*)l;
	_ring_msg = new RingBuffer( _RINGBUFFERSIZE );

	SendDlgItemMessage( hDlg, IDC_MESSAGE   , EM_SETLIMITTEXT, BUFFERSIZE_TEXT    , 0 );
	SendDlgItemMessage( hDlg, IDC_TALKWINDOW, EM_SETLIMITTEXT, BUFFERSIZE_TALKVIEW, 0 );

	{
		char title[ MAX_HOSTNAME ];
		PortNo port;
		sprintf( title, "Port: %d", port.port_no );
		SetWindowText( hDlg, title );
	}

	WindowRect_Load( hDlg, _rect_name, FALSE );

	InitializeCriticalSection( &_cs_talk_window );

	unsigned int id;

	_b_stop     = false;
	_order.func = _NFUNC_Ready;
	_h_thrd     = (HANDLE)_beginthreadex( NULL, 0, _Thread_Table_Server, (LPVOID)_p_server, 0, &id );
//	_Thread_Table_Server( _p_server );

	_Timer_Set( hDlg );
}

static void _WM_CLOSE( HWND hDlg, wsServer *p_server )
{
	_b_stop = true;

	p_server->Shutdown();

	_IDC_Enable( hDlg, IDCANCEL, false );
	RemoveMenu( GetSystemMenu( hDlg, FALSE ), SC_CLOSE, MF_BYCOMMAND );
	DrawMenuBar( hDlg );
	WindowRect_Save( hDlg, _rect_name );

	DeleteCriticalSection( &_cs_talk_window );
}


BOOL CALLBACK dlg_Table( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	switch( msg )
	{
	case WM_INITDIALOG: _WM_INITDIALOG( hDlg, l         ); break;
	case WM_CLOSE     : _WM_CLOSE     ( hDlg, _p_server ); break;
	case WM_TIMER     : _WM_TIMER     ( hDlg, _p_server ); break;

	case WM_COMMAND   :

		switch( LOWORD(w) )
		{
		case IDCANCEL    : _WM_CLOSE    ( hDlg, _p_server ); break;
		case IDC_BTN_SEND: _IDC_BTN_SEND( hDlg, _p_server ); break;

		case IDC_MESSAGE :

			if( HIWORD(w) != EN_SETFOCUS ) break;
			PostMessage( GetDlgItem( hDlg, IDC_MESSAGE ), EM_SETSEL, 0, -1 );
			break;
		}
		break;

	case WM_CTLCOLORSTATIC:

		if( GetDlgItem( hDlg, IDC_TALKWINDOW ) == (HWND)l )
		{
			SetTextColor( (HDC)w, GetSysColor( COLOR_WINDOWTEXT ) );
			SetBkColor  ( (HDC)w, RGB( 240, 240, 240 ) );
			return (BOOL)GetStockObject( WHITE_BRUSH );
		}
		break;

	default: return FALSE;
	}

	return TRUE;
}

