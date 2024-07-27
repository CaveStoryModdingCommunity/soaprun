#include <StdAfx.h>

#include "CriticalSection.h"

CriticalSection::CriticalSection( void )
{
	InitializeCriticalSection( &_cs );
	_b = true;
}
CriticalSection::~CriticalSection( void )
{
	if( _b )
	{
		_b = false;
		DeleteCriticalSection( &_cs );
	}
}

bool CriticalSection::In()
{
	if( _b ) EnterCriticalSection( &_cs );
	return _b;
}

void CriticalSection::Out()
{
	if( _b ) LeaveCriticalSection( &_cs );
}

void CriticalSection::End()
{
	_b = false;
}
