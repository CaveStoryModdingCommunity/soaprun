#include "StdAfx.h"

#include "AppExisting.h"

AppExisting::AppExisting( void )
{
	_hMapping = NULL;
	_hMutex   = NULL;
	_p_hwnd   = NULL;
}

AppExisting::~AppExisting( void )
{
	if( _hMutex   ) ReleaseMutex   ( _hMutex   );
	if( _hMapping ) CloseHandle    ( _hMapping );
	if( _p_hwnd   ) UnmapViewOfFile( _p_hwnd   );
}


HANDLE AppExisting::_CreateMapping( const char *name )
{
	return CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(HWND), name );
}

HWND *AppExisting::_GetMapped( HANDLE h_map )
{
	return (HWND *)MapViewOfFile( h_map, FILE_MAP_WRITE, 0, 0, sizeof(HWND) );
}

bool AppExisting::IsExisting( const char *mutex_name, const char *mapping_name )
{
	if( !OpenMutex( MUTEX_ALL_ACCESS, FALSE, mutex_name ) ) return false;

	HANDLE  h_map   = NULL;
	HWND    *p_hwnd = NULL;

	if( !( h_map = _CreateMapping( mapping_name ) ) ) goto End;
	if( GetLastError() != ERROR_ALREADY_EXISTS      ) goto End;

	if( p_hwnd = _GetMapped( h_map ) )
	{
		if( IsWindowVisible( *p_hwnd ) )
		{
			SetForegroundWindow( *p_hwnd            );
			ShowWindow         ( *p_hwnd, SW_NORMAL );
		}
		UnmapViewOfFile( p_hwnd );
	}

End:
	if( h_map ) CloseHandle( h_map );

	return true;
}

bool AppExisting::Mapping( HWND hWnd, const char *mutex_name, const char *mapping_name )
{
	bool b_ret = false;

	if( !( _hMutex   = CreateMutex( FALSE, 0, mutex_name ) ) ) goto End;
	if( !( _hMapping = _CreateMapping( mapping_name )      ) ) goto End;
	if( !( _p_hwnd   = _GetMapped( _hMapping )             ) ) goto End;

	*_p_hwnd = hWnd;

	b_ret = true;
End:
	if( !b_ret )
	{
		if( _hMutex   ){ ReleaseMutex   ( _hMutex   ); _hMutex   = NULL; }
		if( _hMapping ){ CloseHandle    ( _hMapping ); _hMapping = NULL; }
		if( _p_hwnd   ){ UnmapViewOfFile( _p_hwnd   ); _p_hwnd   = NULL; }
	}

	return TRUE;
}
