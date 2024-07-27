#include <sys/wait.h> // waitpid() / signal() / SIG_XXX etc..
#include <pthread.h> // pthread_xxx()

#include "DebugLog.h"

static pthread_t _thread_id = 0; // メインスレッドのスレッドID

typedef struct{

	int         no  ;
	const char* text;

}_NUMBERANDTEXT;

#define _MAX_TABLE 10

static const _NUMBERANDTEXT _signal_text[ _MAX_TABLE ] =
{
	{ SIGINT , "INT by keybord" },// 2
	{ SIGQUIT, "stop by keybord"},// 3
	{ SIGILL , "ill"            },// 4
	{ SIGABRT, "stop by abort"  },// 6
	{ SIGFPE , "fpe"            },// 8
	{ SIGKILL, "kill"           },// 9
	{ SIGSEGV, "invalid memory" },//11
	{ SIGPIPE, "invalid pipe"   },//13
	{ SIGALRM, "timer by alarm" },//14
	{ SIGTERM, "term"           },//15
};


void Loop_Main_Kill();

static void _call_by_signal( int signum )
{
	int i;
	const char* p_text = "unknown";

	for( i = 0; i < _MAX_TABLE; i++ ){ if( signum == _signal_text[ i ].no ) break; }
	if( i < _MAX_TABLE ) p_text = _signal_text[ i ].text;

	dlog( "signal exit [ %s ] %s", p_text, ( pthread_self() == _thread_id ) ? "main" : "sub" );

	Loop_Main_Kill();
//	Application_Exit();

	pthread_detach( pthread_self() );
	pthread_exit( NULL );
}

void Signal_Set( void )
{
	_thread_id = pthread_self();

	signal( SIGINT , &_call_by_signal );
	signal( SIGQUIT, &_call_by_signal );
	signal( SIGILL , &_call_by_signal );
	signal( SIGABRT, &_call_by_signal );
	signal( SIGFPE , &_call_by_signal );
	signal( SIGKILL, &_call_by_signal );
	signal( SIGSEGV, &_call_by_signal );
	signal( SIGPIPE, &_call_by_signal );
	signal( SIGALRM, &_call_by_signal );
	signal( SIGTERM, &_call_by_signal );
}

