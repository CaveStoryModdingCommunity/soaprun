#include <StdAfx.h>

#include "StringBuffer.h"


bool EditBox_AppendString( HWND hDlg, UINT idc, const char* str )
{
	int  size = SendDlgItemMessage( hDlg, idc, EM_GETLIMITTEXT, 0, 0 );
	char *p   = (char*)malloc(size);

	if( !p ) return false;

	memset( p, 0, size );
	GetDlgItemText( hDlg, idc, p, size );
	StringBuffer_AppendWithReturn( p, size, str );
	SetDlgItemText( hDlg, idc, p );
	SendMessage( GetDlgItem( hDlg, idc), WM_VSCROLL, SB_BOTTOM, 0 );
	free( p );

	return true;
}