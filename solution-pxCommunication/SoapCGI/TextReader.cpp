#include <StdAfx.h>

#include "../Fixture/ShiftJIS.h"

#include "TextReader.h"

TextReader::TextReader( const char *p_buf, int size )
{
	_p_buf = p_buf;
	_size  = size ;
	_c     =     0;
}

bool TextReader::toFind( const char* str )
{
	int  len = strlen( str );

	for( _c; _c < _size; _c++ )
	{
		if( !memcmp( &_p_buf[ _c ], str, len ) ) return true;
//			if( _p_buf[ _c ] & 0x80 ) _c++;
		if( ShiftJIS_Is( _p_buf[ _c ] ) ) _c++;
	}
	return false;
}

bool TextReader::toNextLine( void )
{
	for( _c; _c < _size; _c++ )
	{
		if( _p_buf[ _c ] == '\r' )
		{
			_c++;
			if( _p_buf[ _c ] == '\n' ) _c++;
			return true;
		}
		else if( _p_buf[ _c ] == '\n' )
		{
			_c++;
			return true;
		}
//			if( _p_buf[ _c ] & 0x80 ) _c++;
		if( ShiftJIS_Is( _p_buf[ _c ] ) ) _c++;
	}
	return false;
}

bool TextReader::GetOneLine( char *dst, int size )
{
	int d = 0;
	for( _c; _c < _size && d < size - 1; _c++, d++ )
	{
		if( _p_buf[ _c ] == '\r' )
		{
			_c++;
			if( _p_buf[ _c ] == '\n' ) _c++;
			break;
		}
		else if( _p_buf[ _c ] == '\n' )
		{
			_c++;
			break;
		}
//			else if( _p_buf[ _c ] & 0x80 )
		else if( ShiftJIS_Is( _p_buf[ _c ] ) )
		{
			dst[ d ] = _p_buf[ _c ];
			d++; _c++;
			if( _c >= _size || d >= size - 1 ) break;
		}
		dst[ d ] = _p_buf[ _c ];
	}
	dst[ d ] = '\0';
	return d ? true : false;
}
