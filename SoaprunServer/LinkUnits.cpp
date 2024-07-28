#include <stdlib.h>     // mode_t / exit() / malloc() / atoi()
#include <string.h>     // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <errno.h>      // errno

#ifdef NDEBUG

#include <winsock2.h>  // shutdown()
#include <io.h>        // close()
#define SHUT_RDWR SD_BOTH

#else

#include <sys/socket.h> // shutdown()
#include <unistd.h>     // close()

#endif

#include "SharedDefines.h"
#include "DebugLog.h"
#include "LinkUnits.h"


#ifdef NDEBUG

bool LinkUnits::_mtx_lock  (){ return true; }
void LinkUnits::_mtx_unlock(){              }

#else

bool LinkUnits::_mtx_lock  (){ if( pthread_mutex_lock  ( &_mtx ) && _b_exit ) return false; return true; }
void LinkUnits::_mtx_unlock(){     pthread_mutex_unlock( &_mtx ); }

#endif

LinkUnits::LinkUnits( int max_unit )
{
	_max_login_counter = 0;
	_units = (LINKUNIT*)malloc( sizeof(LINKUNIT) * max_unit );
	memset( _units, 0,          sizeof(LINKUNIT) * max_unit );
	_max_unit = max_unit;
	_b_exit   = false   ;

#ifdef NDEBUG
#else
	memset( &_mtx, 0, sizeof(pthread_mutex_t) );
	pthread_mutex_init( &_mtx, NULL );
#endif

}
LinkUnits::~LinkUnits()
{
	if( _mtx_lock() )
	{
		_b_exit = true;
		if( _units ) free( _units );
	}
#ifdef NDEBUG
#else
	_mtx_unlock();
	pthread_mutex_destroy( &_mtx );
#endif
}

bool LinkUnits::Push( const char *ip, const char *id, int sock )
{
	if( !sock || !ip || !id || !strlen( ip ) || !strlen( id ) ) return false;
	
	bool b_ret = false;
	
	if( _mtx_lock() )
	{
		int u;
		for( u = 0; u < _max_unit; u++ )
		{
			LINKUNIT *p = &_units[ u ];

			if( !p->b_use && !p->sock )
			{
				p->sock  = sock;
				p->b_use = true;
				strcpy( p->host_address, ip );
				strcpy( p->host_id     , id );
				b_ret    = true;
				break;
			}
		}

		// for log..
		int count = 0;
		for( u = 0; u < _max_unit; u++ ){ if( _units[ u ].sock ) count++; }
		if( count > _max_login_counter ) _max_login_counter = count;
		dlog( "total socks %d (max %d)", count, _max_login_counter );
	}
	_mtx_unlock();

	return b_ret;
}

const LINKUNIT *LinkUnits::GetLink( int index )
{
	const LINKUNIT *p_link = NULL;
	
	if( _mtx_lock() )
	{
		LINKUNIT *p = &_units[ index ];
		if( p->b_use && p->sock ) p_link = p;
	}
	_mtx_unlock();

	return p_link;
}

bool LinkUnits::RemoveBeforeClose( int index, const LINKUNIT *p_link )
{
	bool b_ret = false;
	
	if( _mtx_lock() )
	{
		LINKUNIT *p = &_units[ index ];
		if     ( p != p_link ) dlog( "a[%2d] fatal! 1", index ); 
		else if( !p->b_use   ) dlog( "a[%2d] fatal! 2", index );
		else { p->b_use = false; b_ret = true; }
	}
	_mtx_unlock();

	return b_ret;
}

void LinkUnits::ShutdownAndClose( int index, const LINKUNIT *p_link )
{
	if( _mtx_lock() )
	{
		LINKUNIT *p = &_units[ index ];

		if     ( p != p_link ) dlog( "a[%2d] fatal! 3", index ); 
		else if( p->b_use    ) dlog( "a[%2d] fatal! 4", index );
		else
		{
			if( shutdown( p->sock, SHUT_RDWR ) ) dlog( "a[%2d]shut: " "errno = %d!", index, errno ); 
			if( close   ( p->sock            ) ) dlog( "a[%2d]close: ""errno = %d!", index, errno );
			p->sock = 0;

			// for log..
			int count = 0;
			for( int u = 0; u < _max_unit; u++ ){ if( _units[ u ].sock ) count++; }
			dlog( "leave socks %d (max %d)", count, _max_login_counter );
		}
	}
	_mtx_unlock();
}
