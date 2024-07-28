#include <limits.h>   // PATH_MAX / NAME_MAX
#include <stdio.h>    // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <stdlib.h>    // mode_t / exit() / malloc() / atoi()
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()

#ifdef NDEBUG
#else

#define MAX_PATH (PATH_MAX+NAME_MAX)

#endif

#include "dt_MapData.h"
#include "DebugLog.h"


extern char g_dir_data[];

#define _MAX_MAPWIDTH  32767
#define _MAX_MAPLENGTH 32767


static bool _malloc_zero( void **pp, long size )
{
	*pp = malloc( size ); if( !( *pp ) ) return false;
	memset( *pp, 0, size );              return true;
}

static void _free( void **pp )
{
//	dlog( "dt map data" );
	if( *pp ){ free( *pp ); *pp = NULL; }
}


#ifdef NDEBUG

bool dt_MapData::_mtx_lock  (){ return true; }
void dt_MapData::_mtx_unlock(){  }

#else

bool dt_MapData::_mtx_lock  (){ if( pthread_mutex_lock  ( &_mtx ) && _b_exit ) return false; return true; }
void dt_MapData::_mtx_unlock(){     pthread_mutex_unlock( &_mtx ); }

#endif


/////////////////
// グローバル
/////////////////

dt_MapData::dt_MapData( int max_w, int max_l )
{
	_p           = NULL;
	_max_w       = max_w;
	_max_l       = max_l;
	_w           = 0;
	_l           = 0;

	if( max_w > _MAX_MAPWIDTH  ) return;
	if( max_l > _MAX_MAPLENGTH ) return;
	if( !_malloc_zero( (void**)&_p, max_w * max_l ) ) return;

#ifdef NDEBUG
#else
	memset( &_mtx, 0, sizeof(pthread_mutex_t) );
	pthread_mutex_init( &_mtx, NULL );
#endif

}

dt_MapData::~dt_MapData()
{
	if( _mtx_lock() )
	{
		_b_exit = true;
		_free( (void**)&_p );
		_max_w = 0;
		_max_l = 0;
		_w     = 0;
		_l     = 0;
	}

#ifdef NDEBUG
#else
	_mtx_unlock();
	pthread_mutex_destroy( &_mtx );
#endif

}

void dt_MapData::_Clear( void )
{
	if( _p && _max_w && _max_l )
	{
		memset( _p, 0, _max_w * _max_l );
		_w     = 0;
		_l     = 0;
	}
}


unsigned char dt_MapData::SetCorpse( int x, int y, bool b )
{
	unsigned char mparts = 0;
	if( _mtx_lock() )
	{
		if( x < _w && y < _l )
		{
			unsigned char *p = &_p[ y * _max_w + x ];

			if( b )
			{
				if( !( ( (*p) >> 4 ) % 2 ) ){ *p += 0x10; mparts = *p; }
			}
			else
			{
				if(  ( ( (*p) >> 4 ) % 2 ) ){ *p -= 0x10; mparts = *p; }
			}

//			_p[ y * _max_w + x ] = parts;
		}
	}
	_mtx_unlock();
	return mparts;
}


bool dt_MapData::SetMapParts( int x, int y, unsigned char mparts )
{
	bool b_ret = false;
	if( _mtx_lock() )
	{
		if( x < _w && y < _l )
		{
			unsigned char *p = &_p[ y * _max_w + x ];
			if( *p     < 0x20 && ( (*p    ) & 0xf ) >= 12 &&
				mparts < 0x10 && ( (mparts) & 0xf ) >= 12)
			{
				*p = mparts;
				b_ret = true;
			}
		}
	}
	_mtx_unlock();
	return b_ret;
}

unsigned char dt_MapData::GetParts( int x, int y ) const
{
	unsigned char ret = 0;

//	if( _mtx_lock() )
//	{
		if( x < _w && y < _l ) ret = _p[ y * _max_w + x ];
//	}
//	_mtx_unlock();
	return ret;
}

bool dt_MapData::SetSize( int w, int l, int align_flags )
{
	bool b_ret    = false;
	int  offset_x =     0;
	int  offset_y =     0;
	int  x_dst;
	int  y_dst;

	if( !_mtx_lock() ) goto End;
	if( w > _max_w   ) goto End;
	if( l > _max_l   ) goto End;

//	if( align_flags & MAPALIGNFLAG_RIGHT  ) offset_x = w - _w;
//	if( align_flags & MAPALIGNFLAG_BOTTOM ) offset_y = l - _l;

	// フレーム外を掃除
	for( int y = 0; y < _max_l; y++ )
	{
		for( int x = 0; x < _max_w; x++ )
		{
			if( x >= _w || y >= _l ) _p[ y * _max_w + x ] = 0;
		}
	}


	if( offset_x < 0 )
	{
		for( int y_src = 0; y_src < _l; y_src++ )
		{
			for( int x_src = 0; x_src < _w; x_src++ )
			{
				x_dst = x_src + offset_x;
				if( x_dst >= 0 )
				{
					_p[ y_src * _max_w + x_dst ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}
	else if( offset_x > 0 )
	{
		for( int y_src = 0; y_src < _l; y_src++ )
		{
			for( int x_src = _w - 1; x_src >= 0; x_src-- )
			{
				x_dst = x_src + offset_x;
				if( x_dst < _max_w )
				{
					_p[ y_src * _max_w + x_dst ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}

	if( offset_y < 0 )
	{
		for( int y_src = 0; y_src < _l; y_src++ )
		{
			y_dst = y_src + offset_y;
			if( y_dst >= 0 )
			{
				for( int x_src = 0; x_src < _w; x_src++ )
				{
					_p[ y_dst * _max_w + x_src ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}
	else if( offset_y > 0 )
	{
		for( int y_src = _l - 1; y_src >= 0; y_src-- )
		{
			y_dst = y_src + offset_y;
			if( y_dst < _max_l )
			{
				for( int x_src = 0; x_src < _w; x_src++ )
				{
					_p[ y_dst * _max_w + x_src ] = _p[ y_src * _max_w + x_src ];
					_p[ y_src * _max_w + x_src ] = 0;
				}
			}
		}
	}

	_w = w;
	_l = l;
	b_ret = true;
End:
	_mtx_unlock();

	return b_ret;
}


bool dt_MapData::Load( const char *name )
{
	bool           b_ret = false;
	FILE           *fp = NULL;
	char           path[ MAX_PATH ];
	unsigned short us;

	if( !_mtx_lock() ) goto End;


	_Clear();


	sprintf( path, "%s/%s", g_dir_data, name );
	if( !( fp = fopen( path, "rb" ) ) ) goto End;

	printf( "map load: %s\n", path );

	if( fread( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End; _w = us;
	if( fread( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End; _l = us;

	if( _w > _max_w || _l > _max_l ) goto End;

	printf( "map ok\n" );

	for( int y = 0; y < _l; y++ )
	{
		if( fread( &_p[ y * _max_w ], sizeof(unsigned char), _w, fp ) != _w ) goto End;
	}

	b_ret = true;
End:

	if( fp     ) fclose( fp );
	if( !b_ret ) _Clear();

	_mtx_unlock();

	return b_ret;
}

bool dt_MapData::Save( const char *name )
{
	bool           b_ret = false;
	FILE           *fp = NULL;
	char           path[ MAX_PATH ];
	unsigned short us;

	if( !_mtx_lock() ) goto End;


	sprintf( path, "%s/%s", g_dir_data, name );
	if( !( fp = fopen( path, "wb" ) ) ) goto End;

	printf( "map save: %s\n", path );

	us = _w; if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;
	us = _l; if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;

	for( int y = 0; y < _l; y++ )
	{
		if( fwrite( &_p[ y * _max_w ], sizeof(unsigned char), _w, fp ) != _w ) goto End;
	}

	printf( "map save: ok.\n" );
	b_ret = true;
End:

	if( fp     ) fclose( fp );

	_mtx_unlock();

	return b_ret;
}

//bool dt_MapData::Write( FILE* fp )
//{
//	bool           b_ret  = false;
////	bool           b_comp = false;
//	unsigned short us;
////	unsigned char  uc;
///*
//	{
//		int compress_size = 0;
//
//		int x = 0;
//		int y = 0;
//		
//		while( y < _l )
//		{
//			us = 0;
//			uc = _p[ y * _max_w + x ];
//			while( y < _l && us < 0xffff && _p[ y * _max_w + x ] == uc )
//			{
//				us++;
//				if( ++x >= _w ){ x = 0; ++y; }
//			}
//			compress_size += 3;
//		}
//		if( compress_size < _w * _l ) b_comp = true;
//	}
//
//	if( b_comp ) uc = 1;
//	else         uc = 0;
//	if( fwrite( &uc, sizeof(unsigned char ), 1, fp ) != 1 ) goto End;
//*/
//	us = (unsigned short)_w;
//	if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;
//
//	us = (unsigned short)_l;
//	if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;
//
//
////	if( !b_comp )
////	{
//		for( int y = 0; y < _l; y++ )
//		{
//			if( fwrite( &_p[ y * _max_w ], sizeof(unsigned char), _w, fp ) != _w ) goto End;
//		}
///*	}
//	else
//	{
//		int x = 0;
//		int y = 0;
//		
//		while( y < _l )
//		{
//			us = 0;
//			uc = _p[ y * _max_w + x ];
//			while( y < _l && us < 0xffff && _p[ y * _max_w + x ] == uc )
//			{
//				us++;
//				if( ++x >= _w ){ x = 0; ++y; }
//			}
//			if( fwrite( &us, sizeof(unsigned short), 1, fp ) != 1 ) goto End;
//			if( fwrite( &uc, sizeof(unsigned char ), 1, fp ) != 1 ) goto End;
//		}
//	}
//*/
//	b_ret = true;
//End:
//
//	return b_ret;
//}

bool dt_MapData::GetClip( dt_MapData *dst, int src_x, int src_y )
{
	bool b_ret = false;

	if( !_mtx_lock() ) goto End;

	if( src_x < 0 || src_y < 0 ) goto End;
	if( src_x + dst->_w > _w   ) goto End;
	if( src_y + dst->_l > _l   ) goto End;

	{
		const unsigned char *p_src = &_p[ src_x + src_y * _max_w ];
		unsigned       char *p_dst =  dst->_p;

		for( int y = 0; y < dst->_l; y++ )
		{
			for( int x = 0; x < dst->_w; x++ ) *p_dst++ = *p_src++;
			p_src += _max_w -       dst->_w;
		}
	}
	b_ret = true;
End:
	_mtx_unlock();
	return b_ret;
}

bool dt_MapData::CopyTo( dt_MapData *dst )
{
	bool b_ret = false;
	if( _mtx_lock() && _w == dst->_w || _l == dst->_l )
	{
		const unsigned char *p_src = _p;
		unsigned       char *p_dst = dst->_p;
		memcpy( dst->_p, _p, _w * _l );
		b_ret = true;
	}
	_mtx_unlock();
	return b_ret;
}

void dt_MapData::Fill( unsigned char uc )
{
	if( _mtx_lock() ) memset( _p, uc, _w * _l );
	_mtx_unlock();
}
