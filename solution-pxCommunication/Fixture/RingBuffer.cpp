#include <StdAfx.h>

#include "ShiftJIS.h"

#include "RingBuffer.h"

void RingBuffer::Clear()
{
	EnterCriticalSection( &_cs );
	memset( _p_buf, 0, _size );
	memset( _text , 0, _size );
	_buf_r  = 0;
	_buf_w  = 0;
	_w_loop = 0;
	_r_loop = 0;

	_b_overwrite = false;
	_b_false     = false;
	LeaveCriticalSection( &_cs );
}

RingBuffer::RingBuffer( int size )
{
	InitializeCriticalSection( &_cs );
	_p_buf = (unsigned char*)malloc( size );
	_text  = (         char*)malloc( size );
	_size  = size;
	RingBuffer::Clear();
}

RingBuffer::~RingBuffer( void )
{
	free( _p_buf );
	free( _text  );
	DeleteCriticalSection( &_cs );
}

unsigned char RingBuffer::_Read()
{
	unsigned char uc = _p_buf[ _buf_r ];
	if( ++_buf_r >= _size ){ _buf_r = 0; _r_loop++; };
	return uc;
}

int RingBuffer::_Read_Length()
{
	int len = 0;
	int r   = _buf_r;
//	int w   = _buf_r;

	if( r >= _size ) r -= _size; len += _p_buf[ r ] * 0x1      ; r++;
	if( r >= _size ) r -= _size; len += _p_buf[ r ] * 0x100    ; r++;
	if( r >= _size ) r -= _size; len += _p_buf[ r ] * 0x10000  ; r++;
	if( r >= _size ) r -= _size; len += _p_buf[ r ] * 0x1000000;

	if( len )
	{
		if( ++_buf_r >= _size ){ _buf_r -= _size; _r_loop++; }
		if( ++_buf_r >= _size ){ _buf_r -= _size; _r_loop++; }
		if( ++_buf_r >= _size ){ _buf_r -= _size; _r_loop++; }
		if( ++_buf_r >= _size ){ _buf_r -= _size; _r_loop++; }
	}

	return len;
}

void RingBuffer::_Write( unsigned char uc )
{
/*	if( _buf_w == _buf_r )
	{
		int len = _Read_Length();
		if( len )
		{
			_b_overwrite = true;
			_buf_r += len;
			if( _buf_r >= _size ){ _buf_r -= _size; _r_loop++; }
		}
	}
*/	_p_buf[ _buf_w ] = uc;
	if( ++_buf_w >= _size ){ _buf_w = 0; _w_loop++; }
}

void RingBuffer::_Write_Length( int len )
{
	unsigned char uc;
	uc =   len         & 0xff; _Write( uc );
	uc = ( len >> 8  ) & 0xff; _Write( uc );
	uc = ( len >> 16 ) & 0xff; _Write( uc );
	uc = ( len >> 24 ) & 0xff; _Write( uc );
}

bool RingBuffer::txt_Push( const char* fmt, ... )
{
	bool b_ret = false;

	EnterCriticalSection( &_cs );

	va_list ap; va_start( ap, fmt ); vsprintf( _text, fmt, ap ); va_end( ap );
	int len = strlen( _text );

	int left;
	if( _buf_r > _buf_w ) left = _buf_r - _buf_w        ;
	else                  left = _size - _buf_w + _buf_r;
	if( left < len + 4 ){ _b_false = true; goto End; }

/*
	if( !len ) goto End;

	if( len > _size - 4 )
	{
		len = _size - 4;
		if( ShiftJIS_Is( _text[ len - 1 ] ) ) len--;
	}
*/
	_Write_Length( len );

	for( int i = 0; i < len; i++ ) _Write( _text[ i ] );
	b_ret = true;
End:
	LeaveCriticalSection( &_cs );

	return b_ret;
}

bool RingBuffer::txt_Pop( char *text )
{
	bool b_ret = false;

	EnterCriticalSection( &_cs );

	if( !text ) goto End;

	text[ 0 ] = '\0';

	if( _buf_r == _buf_w && _w_loop == _r_loop ) goto End;

	int len = _Read_Length();

	int i = 0;
	for( i; i < len; i++ ) text[ i ] = _Read();
	text[ i ] = '\0';

	b_ret = true;
End:

	LeaveCriticalSection( &_cs );
	return b_ret;
}

bool RingBuffer::bin_Push( const void *p, int size )
{
	bool b_ret = false;

	EnterCriticalSection( &_cs );

	if( !size || !p ) goto End;

//	int left;
/*	if( _buf_r > _buf_w ) left = _buf_r - _buf_w        ;
	else                  left = _size - _buf_w + _buf_r;
*/
	int left = _size - 
		( _w_loop * _size + _buf_w ) +
		( _r_loop * _size + _buf_r );

	if( left < size + 4 ){ _b_false = true; goto End; }

//	if( size > _size - 4 ) size = _size - 4;

	_Write_Length( size );

	unsigned char *p2 = (unsigned char*)p;

	for( int i = 0; i < size; i++ ) _Write( *p2++ );
	b_ret = true;
End:
	LeaveCriticalSection( &_cs );
	return b_ret;
}

int RingBuffer::bin_Pop( void *p )
{
	bool b_ret = false;
	int  len   =     0;

	EnterCriticalSection( &_cs );

	if( !p ) goto End;

	if( _buf_r == _buf_w && _w_loop == _r_loop ) goto End;

	len = _Read_Length();

	unsigned char *p2 = (unsigned char*)p;

	for( int i = 0; i < len; i++ ) *p2++ = _Read();

	b_ret = true;
End:

	LeaveCriticalSection( &_cs );
	return len;
}

bool RingBuffer::IsPop()
{
	bool b_ret = false;
	EnterCriticalSection( &_cs );
	if( _buf_r != _buf_w || _w_loop != _r_loop ) b_ret = true;
	LeaveCriticalSection( &_cs );
	return b_ret;
}

bool RingBuffer::IsOverwrite()
{
	bool b_ret = false;
	EnterCriticalSection( &_cs );
	if( _b_overwrite ) b_ret = true;
	LeaveCriticalSection( &_cs );
	return b_ret;
}
/*
int RingBuffer::GetUsing()
{
	int size = 0;
	EnterCriticalSection( &_cs );
	if( _buf_r <= _buf_w ) size = _buf_w - _buf_r;
	else                   size = _size  - _buf_r + _buf_w;
	LeaveCriticalSection( &_cs );
	return size;
}

int RingBuffer::GetLeft()
{
	int size = 0;
	EnterCriticalSection( &_cs );
	if( _buf_r >= _buf_w ) size = _buf_r - _buf_w;
	else                   size = _size - _buf_w + _buf_r;
	LeaveCriticalSection( &_cs );
	return size;
}
*/
int RingBuffer::GetUsing()
{
	int size = 0;
	EnterCriticalSection( &_cs );

	size = ( _w_loop * _size + _buf_w ) -
		   ( _r_loop * _size + _buf_r );

	LeaveCriticalSection( &_cs );
	return size;
}

int RingBuffer::GetLeft()
{
	int size = 0;
	EnterCriticalSection( &_cs );

	size = _size - 
		( _w_loop * _size + _buf_w ) +
		( _r_loop * _size + _buf_r );

	LeaveCriticalSection( &_cs );
	return size;
}


int RingBuffer::GetSize()
{
	return _size;
}
