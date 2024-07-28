#include <stdio.h>     // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <limits.h>    // PATH_MAX / NAME_MAX
#include <string.h>    // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <stdlib.h>    // mode_t / exit() / malloc() / atoi()

#ifdef NDEBUG

#include <windows.h>

#else

#include <unistd.h>    // _exit() / readlink() / unlink() / sleep() / setsid() / close() / dup2()
#define MAX_PATH (PATH_MAX+NAME_MAX)

#endif


#include "SharedDefines.h"
#include "Daemon.h"
#include "DebugLog.h"
#include "dt_FldObj.h"
#include "LinkUnits.h"
#include "RequestResponse.h"
#include "Signal.h"
#include "svFieldUnits.h"
#include "svAttender.h"
#include "svListener.h"


#define _MAX_ATTENDER     MAX_CLIENTSOCKET
#define _MAX_PLAYER       MAX_ENABLESOCKET

static const char *_sub_map_name     = "map(sub).bin" ;
static const char *_app_name         = "SorprunServer";
const char        *g_version_text    = "v.0.4.3.3"    ;
const char        *g_date_text       = "2010/05/29"   ;
LinkUnits         *g_link_units      = NULL           ; 
char              g_dir_data[ MAX_PATH ];

dt_MapData        *g_mAttr           = NULL;
dt_MapData        *g_map             = NULL;
svFieldUnits      *g_funits          = NULL;




bool _Directory_data()
{
#ifdef NDEBUG

#else

	int i;
	readlink( "/proc/self/exe", g_dir_data, MAX_PATH );
	for( i = strlen( g_dir_data ) -1; i > 0; i-- )
	{
		if( g_dir_data[ i ] == '/' ){ g_dir_data[ i ] = '\0'; break; }
	}
	if( i == 0 ) return false;
	sprintf( g_dir_data, "%s/data", g_dir_data );
#endif
	return true;
}


typedef struct{ char  a; char  b; unsigned char c; } STRUCT_1; // 3
typedef struct{ short a;                   } STRUCT_2; // 2
typedef struct{ char  a;                   } STRUCT_3; // 1
typedef struct{ char  a; short b;          } STRUCT_4; // 4
typedef struct{ short a; int   b;          } STRUCT_5; // 8
typedef struct{ int   a; short b;          } STRUCT_6; // 8
typedef struct{ char  a; int   b;          } STRUCT_7; // 8
typedef struct{ char  a; short b; char  c; } STRUCT_8; // 6
typedef struct{ char  a; char  c; short b; } STRUCT_9; // 4
typedef struct{ char  a; int   b; char  c; } STRUCT_a; //12
typedef struct{ char  a; unsigned char  c; int   b; } STRUCT_b; // 8
typedef struct{ char req_name[ 4 ]; bool  b_result; }STRUCT_c     ;

int main( int argc, char* argv[] )
{
	{
		if( sizeof(STRUCT_1) !=  3 ) return 1;
		if( sizeof(STRUCT_2) !=  2 ) return 1;
		if( sizeof(STRUCT_3) !=  1 ) return 1;
		if( sizeof(STRUCT_4) !=  4 ) return 1;
		if( sizeof(STRUCT_5) !=  8 ) return 1;
		if( sizeof(STRUCT_6) !=  8 ) return 1;
		if( sizeof(STRUCT_7) !=  8 ) return 1;
		if( sizeof(STRUCT_8) !=  6 ) return 1;
		if( sizeof(STRUCT_9) !=  4 ) return 1;
		if( sizeof(STRUCT_a) != 12 ) return 1;
		if( sizeof(STRUCT_b) !=  8 ) return 1;
		if( sizeof(STRUCT_c) !=  5 ) return 1;
	}

	bool       bDaemon = false;
	svListener *lstn   = NULL;
	svAttender *atnds[ _MAX_ATTENDER ];

	memset( atnds, 0, sizeof(svAttender*) * _MAX_ATTENDER );

	_Directory_data();


	// option..
	if( argc > 1 )
	{
		if(      !strcmp( argv[ 1 ], "-D"  ) )
		{
			printf( "%s %s (%s) can't daemon\n", _app_name, g_version_text, g_date_text );
			goto End;
			bDaemon = true;
		}
		else if( !strcmp( argv[ 1 ], "--version" ) )
		{
			printf( "%s %s (%s)\n", _app_name, g_version_text, g_date_text );
			goto End;
		}
		else if( !strcmp( argv[ 1 ], "--port" ) )
		{
			g_port_no = atoi( argv[ 2 ] );	
		}
	}

	// debug log
	if( !DebugLog_Initialize( bDaemon ) ) return 1;
	if( argc > 1 ) dlog( "argv[ 1 ] : %s", argv[ 1 ] );

	dlog( "## start %s %s (%s) ##", _app_name, g_version_text, g_date_text );

	// Attribute
	g_mAttr = new dt_MapData(  16,  16 );
	if( !g_mAttr->Load( "mparts.pxattr" ) ){ dlog( "mAttr: Err." ); goto End; }
	else                                     dlog( "mAttr: Ok."  );

	// Map
	g_map   = new dt_MapData( MAX_FIELDSIZE_W, MAX_FIELDSIZE_L );
	if( !g_map  ->Load( _sub_map_name ) )
	{
		if( !g_map->Load( "map.bin" ) ){ dlog( "map: Err."   ); goto End; }
		else                             dlog( "map: Ok. %d/%d", g_map->get_w(), g_map->get_l() );
	}
	else
	{
		dlog( "map(sub): Ok. %d/%d", g_map->get_w(), g_map->get_l() );
	}

	g_funits = new svFieldUnits( _MAX_PLAYER, MAX_NPU, g_map, g_mAttr );

	// NPU.
	{
		dt_FldObj fobj( MAX_NPU );
		if( !fobj.Load( "unit.bin" ) ){ dlog( "fobj: Err." ); goto End; }
		int num = fobj.Count();
		dlog( "fobj: %d Ok.", num );
		const DT_FLDOBJ *p;
		for( int u = 0; u < num; u++ ){ p = fobj.Get( u ); g_funits->ReadyNPU( u, p->param1, p->param2, p->x, p->y ); }
	}

	if( bDaemon && !Daemon_Set() ) goto End;

	g_link_units = new LinkUnits( MAX_CLIENTSOCKET );

#ifdef NDEBUG
#else
	if( !Signal_Mask( NULL )   ) goto End;
	if( !Signal_CreateThread() ) goto End;
#endif

	lstn = new svListener( g_port_no );
	for( int a = 0; a < _MAX_ATTENDER; a++ )
	{
		bool b_over_quota = false;
		if( a >= MAX_ENABLESOCKET ) b_over_quota = true;
		atnds[ a ] = new svAttender( a, b_over_quota );
	}

	lstn->WaitExit();
	for( int a = 0; a < _MAX_ATTENDER; a++ ) atnds[ a ]->WaitExit(); 

#ifdef NDEBUG
#else
	Signal_JoinThread();
#endif

	dlog( "normal exit" );

End:

	for( int a = 0; a < _MAX_ATTENDER; a++ ){ if( atnds[ a ] ) delete atnds[ a ]; }
	if( lstn         ) delete lstn        ;
	if( g_link_units ) delete g_link_units;

	if( g_map        )
	{
		g_map->Save( _sub_map_name );
		delete g_map;
	}

	if( g_mAttr      ) delete g_mAttr     ;
	if( g_funits     ) delete g_funits    ;

	DebugLog_Release();

	return 0;
}
