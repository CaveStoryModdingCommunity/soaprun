#include <StdAfx.h>

void WindowFlash( HWND hDlg )
{
	FLASHWINFO flash;
	flash.cbSize    = sizeof(FLASHWINFO);
	flash.dwTimeout = 0;
	flash.hwnd      = hDlg;
	flash.uCount    = 3;
	flash.dwFlags   = FLASHW_TIMERNOFG|FLASHW_ALL ;
	FlashWindowEx( &flash );
}