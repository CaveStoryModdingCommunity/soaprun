#include <StdAfx.h>

#include "FontWork.h"
#include "SecondCounter.h"

SecondCounter::SecondCounter( int keta )
{
	_keta = keta;
}

void SecondCounter::Reset( void )
{
	_msec_start = GetTickCount();
}

void SecondCounter::Put  ( const RECT *rc_view, int x, int y )
{
	FontWork_PutNumber6( rc_view, x, y, (GetTickCount() - _msec_start) / 1000, _keta );
}

int SecondCounter::Get()
{
	return (GetTickCount() - _msec_start) / 1000;
}