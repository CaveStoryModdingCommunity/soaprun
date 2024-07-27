#include <StdAfx.h>

//#include <windows.h>
//#include <stdio.h>

#include "ShiftJIS.h"
//			SendMessage( GetDlgItem( hDlg, IDC_EDIT1), WM_VSCROLL, SB_BOTTOM, 0 );

int _strlen( const char* str, int buf_size )
{
	int len = 0;
	for( int c = 0; c < buf_size - 1; c++, len++ )
	{
		if( str[ c ] == '\0' ) break;
//		if( str[ c ] & 0x80  )
		if( ShiftJIS_Is( str[ c ] ) )
		{
			if( c + 1 == buf_size - 1 ) break;
			c++; len++;
		}
	}
	return len;
}

bool StringBuffer_Insert( char *p_buf, int buf_size, const char* str )
{
	int ins_len      = _strlen( str  , buf_size ); if( !ins_len ) return false;
	int move_buf_len = _strlen( p_buf, buf_size - ins_len );
	memcpy( &p_buf[ ins_len ], p_buf, move_buf_len );
	p_buf[ ins_len + move_buf_len ] = '\0';

	memcpy( &p_buf[ 0 ], str  , ins_len      );
	p_buf[ buf_size - 1 ] = '\0';

	return true;
}

bool StringBuffer_InsertWithReturn( char *p_buf, int buf_size, const char* str )
{
	int ins_len = _strlen( str, buf_size ); if( !ins_len ) return false;

	int move;

	if( ins_len + 2 < buf_size ) move = ins_len + 2;
	else                         move = ins_len    ;

	int move_buf_len = _strlen( p_buf, buf_size - move );
	memcpy( &p_buf[ move ], p_buf, move_buf_len );
	p_buf[ move + move_buf_len ] = '\0';

	memcpy( p_buf, str, ins_len );
	if( ins_len + 2 < buf_size ) memcpy( &p_buf[ ins_len ], "\r\n", 2 );
	else                                  p_buf[ ins_len ] = '\0';

	p_buf[ buf_size - 1 ] = '\0';
	return true;
}

bool StringBuffer_Append( char *p_buf, int buf_size, const char* str )
{
	int append_len   = _strlen( str  , buf_size ); if( !append_len ) return false;
	int exist_len    = _strlen( p_buf, buf_size );
	int move_buf_len = ( exist_len + append_len + 1 ) - buf_size;

	if( move_buf_len > 0 )
	{
//		if( p_buf[ move_buf_len - 1 ] & 0x80 )
		if( ShiftJIS_Is( p_buf[ move_buf_len - 1 ] ) )
		{
			move_buf_len++;
		}
		memcpy( p_buf, &p_buf[ move_buf_len ], buf_size - move_buf_len );
		exist_len -= move_buf_len;
	}
	memcpy( &p_buf[ exist_len ], str, append_len );
	p_buf[ exist_len + append_len ] = '\0';

	return true;
}

bool StringBuffer_AppendWithReturn( char *p_buf, int buf_size, const char* str )
{
	int append_len   = _strlen( str  , buf_size - 2 ); if( !append_len ) return false;
	int exist_len    = _strlen( p_buf, buf_size );
	int move_buf_len = ( exist_len + append_len + 3 ) - buf_size;

	if( move_buf_len > 0 )
	{
//		if( p_buf[ move_buf_len - 1 ] & 0x80 )
		if( ShiftJIS_Is( p_buf[ move_buf_len - 1 ] ) )
		{
			move_buf_len++;
		}
		memcpy( p_buf, &p_buf[ move_buf_len ], buf_size - move_buf_len );
		exist_len -= move_buf_len;
	}
	memcpy( &p_buf[ exist_len     ], "\r\n",          2 );
	memcpy( &p_buf[ exist_len + 2 ], str   , append_len );
	p_buf[ exist_len + 2 + append_len ] = '\0';

	return true;
}
