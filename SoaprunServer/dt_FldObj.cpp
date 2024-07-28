#include <limits.h>   // PATH_MAX / NAME_MAX
#include <stdio.h>    // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <stdlib.h>    // mode_t / exit() / malloc() / atoi()
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()

#ifdef NDEBUG

#include <windows.h>

#else

#define MAX_PATH (PATH_MAX+NAME_MAX)

#endif

#include "dt_FldObj.h"
#include "DebugLog.h"

extern char g_dir_data[];

static bool _malloc_zero( void **pp, long size )
{
	*pp = malloc( size ); if( !( *pp ) ) return false;
	memset( *pp, 0, size );              return true ;
}

static void _free( void **pp )
{
//	dlog( "dt fld obj" );
	if( *pp ){ free( *pp ); *pp = NULL; }
}

dt_FldObj::dt_FldObj( int max_unit )
{
	_max_unit = max_unit;
	if( !_malloc_zero( (void**)&_units, sizeof(DT_FLDOBJ) * max_unit ) ) return;
}

dt_FldObj::~dt_FldObj()
{

	_free( (void**)&_units );
}

void dt_FldObj::Clear( void )
{
	if( _units && _max_unit ) memset( _units, 0, sizeof(DT_FLDOBJ) * _max_unit );
}

int dt_FldObj::Count( void )
{
	if( !_units || !_max_unit ) return 0;

	int count = 0;
	for( int u = 0; u < _max_unit; u++ )
	{
		if( _units[ u ].flags & UNITRECORD_FLAG_VALID ) count++;
	}
	return count;
}

bool dt_FldObj::Load( const char *name )
{
	bool  b_ret = false;
	DT_FLDOBJ *p_unit      ;
	short num;

	Clear();

	char path[ MAX_PATH ];
	FILE *fp = NULL;

	sprintf( path, "%s/%s", g_dir_data, name );
	if( !( fp = fopen( path, "rb" ) ) ) goto End;

	printf( "npus load: %s\n", path );

	Clear();
	if( fread( &num, sizeof(short), 1, fp ) != 1 ) goto End;
	if( num > _max_unit ) goto End;

	for( long u = 0; u < num; u++ )
	{	
		p_unit = &_units[ u ];

		if( fread( &p_unit->flags    , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
//		if( fread( &p_unit->unit_type, sizeof(unsigned char), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->param1   , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->param2   , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->x        , sizeof(short        ), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->y        , sizeof(short        ), 1, fp ) != 1 ) goto End;
	}

	b_ret = true;
End:
	if( fp     ) fclose( fp );
	if( !b_ret ) Clear (    );

	return b_ret;
}
/*
bool dt_FldObj::Read( FILE *fp )
{
	bool      b_ret = false;
	DT_FLDOBJ *p_unit      ;
	short     num          ;

	Clear();
	if( fread( &num, sizeof(short), 1, fp ) != 1 ) goto End;
	if( num > _max_unit ) goto End;

	for( long u = 0; u < num; u++ )
	{	
		p_unit = &_units[ u ];

		if( fread( &p_unit->flags    , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
//		if( fread( &p_unit->unit_type, sizeof(unsigned char), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->param1   , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->param2   , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->x        , sizeof(short        ), 1, fp ) != 1 ) goto End;
		if( fread( &p_unit->y        , sizeof(short        ), 1, fp ) != 1 ) goto End;
	}

	b_ret = true;
End:
	if( !b_ret ) Clear();

	return b_ret;
}

bool dt_FldObj::Write( FILE* fp )
{
	bool      b_ret = false;
	DT_FLDOBJ *p_unit;
	short     count = Count();

	if( fwrite( &count, sizeof(short), 1, fp ) != 1 ) goto End;

	for( long u = 0; u < _max_unit; u++ )
	{	
		p_unit = &_units[ u ];

		if( p_unit->flags & UNITRECORD_FLAG_VALID )
		{
			if( fwrite( &p_unit->flags      , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
			if( fwrite( &p_unit->unit_type  , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
			if( fwrite( &p_unit->param1     , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
			if( fwrite( &p_unit->param2     , sizeof(unsigned char), 1, fp ) != 1 ) goto End;
			if( fwrite( &p_unit->x          , sizeof(short        ), 1, fp ) != 1 ) goto End;
			if( fwrite( &p_unit->y          , sizeof(short        ), 1, fp ) != 1 ) goto End;
		}
	}

	b_ret = true;
End:

	return b_ret;
}
*/

void dt_FldObj::Shift( short x, short y )
{
	for( int u = 0; u < _max_unit; u++ )
	{
		if( _units[ u ].flags & UNITRECORD_FLAG_VALID )
		{
			_units[ u ].x += x;
			_units[ u ].y += y;
		}
	}
}

bool dt_FldObj::Add( const DT_FLDOBJ* p_unit )
{
	for( int u = 0; u < _max_unit; u++ )
	{
		if( !(_units[ u ].flags & UNITRECORD_FLAG_VALID) )
		{
			_units[ u ]        = *p_unit;
			_units[ u ].flags |= UNITRECORD_FLAG_VALID;
			return true;
		}
	}
	return false;
}

bool dt_FldObj::Set( const DT_FLDOBJ* p_unit, int index )
{
	if( index >= _max_unit ) return false;

	if( _units[ index ].flags & UNITRECORD_FLAG_VALID )
	{
		memcpy( &_units[ index ], p_unit, sizeof(DT_FLDOBJ) );
		_units[ index ].flags |= UNITRECORD_FLAG_VALID;
		return true;
	}

	return false;
}

const DT_FLDOBJ* dt_FldObj::Get( int index )
{
	if( index >= _max_unit ) return NULL;
	if( _units[ index ].flags & UNITRECORD_FLAG_VALID ) return &_units[ index ];
	return NULL;
}

bool dt_FldObj::Delete( int index )
{
	if( index >= _max_unit ) return false;
	for( int i = index; i < _max_unit - 1; i++ ){ _units[ i ] = _units[ i + 1 ]; }
	_units[                 _max_unit - 1 ].flags = 0;
	return true;
}

void dt_FldObj::Sort_Up( void )
{
	DT_FLDOBJ w;
	int top = Count() - 1;

	while( top )
	{
		for( int u = 0; u < top; u++ )
		{
			if( _units[ u ].y < _units[ u + 1 ].y )
			{
				w               = _units[ u     ];
				_units[ u     ] = _units[ u + 1 ];
				_units[ u + 1 ] = w;
			}
		}
		top--;
	}
}
