#include <StdAfx.h>

#include "Random.h"

#include "DebugLog.h"

Random::Random( unsigned short a, unsigned short b )
{
	_buf[ 0 ] = a;
	_buf[ 1 ] = b;
}

short Random::Get( short min_, short max_ )
{
	long w1, w2;
	char *p1;
	char *p2;

	w1 = (short)( _buf[ 0 ] + _buf[ 1 ] );
	w2 = 0; // 091203 ç°Ç‹Ç≈ñ≥Ç©Ç¡ÇΩÇØÇ«ëÂè‰ïvÇ»ÇÒÇæÇÎÇ§Ç©Åc

	p1 = (char *)&w1;
	p2 = (char *)&w2;

	p2[ 0 ] = p1[ 1 ];
	p2[ 1 ] = p1[ 0 ];

	_buf[ 1 ] = (short)_buf[ 0 ];
	_buf[ 0 ] = (short)w2;

//	return (short)w2;

	short ret = (short)( min_ + w2 % ( max_ - min_ + 1 ) );

#ifdef _DEBUG
//	dlog( "ran: %d", ret );
#endif

	return ret;
}

