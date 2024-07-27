#include "StdAfx.h"

#include "DebugLog.h"

void mbox_ERR( HWND hWnd, const char *fmt, ... )
{
	char str[ 1024 ];

	va_list ap; va_start( ap, fmt ); vsprintf( str, fmt, ap ); va_end( ap );

	dlog( "mbox ERR( %s )", str );
	MessageBox( hWnd, str, "ÉGÉâÅ[", MB_OK|MB_ICONEXCLAMATION );
}
