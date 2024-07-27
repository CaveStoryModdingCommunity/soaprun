#include <StdAfx.h>

bool ShiftJIS_Is( char c )
{
	unsigned char u = (unsigned char)c;
	if( u >= 0x81 && u <= 0x9F ) return true;
	if( u >= 0xE0 && u <= 0xFC ) return true;
	return false;
}
