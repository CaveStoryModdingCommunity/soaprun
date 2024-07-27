#include "StdAfx.h"


// ファイル属性を調べる
static bool _IsNormalFile( WIN32_FIND_DATA* p_find )
{
	if( !strcmp( p_find->cFileName , "."  ) ) return false;
	if( !strcmp( p_find->cFileName , ".." ) ) return false;
	if( p_find->dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_DIRECTORY) ) return false;


	return true;
}
/*
bool FileWork_IsImageExtension( const char* path )
{
	const char* p_ext = PathFindExtension( path );
	if( !stricmp( p_ext, ".tif"  ) ) return true;
	if( !stricmp( p_ext, ".tiff" ) ) return true;
	if( !stricmp( p_ext, ".eps"  ) ) return true;
	return false;
}
*/

// ファイル安定チェック
bool FileWork_IsFileStable( const char *path, int sec_stable )
{
	HANDLE     hFile ;
	FILETIME   ft    ;
	FILETIME   ft_now;
	SYSTEMTIME st    ;


	hFile = CreateFile( path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ) return false;

	GetFileTime( hFile, &ft, NULL, NULL );

	CloseHandle( hFile );

	GetSystemTime(         &st          );
	SystemTimeToFileTime( &st, &ft_now );

	if( ft.dwHighDateTime != ft_now.dwHighDateTime                                ) return true;
	if( ft.dwLowDateTime  <= ft_now.dwLowDateTime - 1000 * 1000 * 10 * sec_stable ) return true;

	return false;
}
/*
// 入力フォルダを調べる
bool FileWork_FindStableImageFile( const char* path_dir, char* path_find, int sec_stable )
{
	HANDLE          hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA find;

	char path[ MAX_PATH ];

	sprintf( path, "%s\\*.*", path_dir );

	if( ( hFind = FindFirstFile( path, &find ) ) != INVALID_HANDLE_VALUE )
	{
		do{
			if( FileWork_IsNormalFile( &find ) && FileWork_IsImageExtension( find.cFileName ) )
			{
				sprintf( path_find, "%s\\%s", path_dir, find.cFileName );
				if( FileWork_IsFileStable( path_find, sec_stable ) )
				{
					FindClose( hFind );
					return true;
				}
			}

		} while( FindNextFile ( hFind, &find ) );

		FindClose( hFind );
	}

	return false;
}
*/

// フォルダが空であるか
bool FileWork_IsDirectoryEmpty( const char* path_dir )
{
	HANDLE          hFind;
	WIN32_FIND_DATA find ;

	char path[ MAX_PATH ];
	int  count = 0;

	sprintf( path, "%s\\*.*", path_dir );

	if( ( hFind = FindFirstFile( path, &find ) ) != INVALID_HANDLE_VALUE )
	{
		do{
			if( _IsNormalFile( &find ) ) count++;
		} while( FindNextFile ( hFind, &find ) );

		FindClose( hFind );
	}

	if( count ) return false;

	return true;
}

// フォルダをクリア
void FileWork_ClearDirectory( const char* path_dir )
{
	HANDLE          hFind;
	WIN32_FIND_DATA find ;

	char path[ MAX_PATH ];

	if( strlen( path_dir ) < 5 ) return;

	sprintf( path, "%s\\*.*", path_dir );

	if( ( hFind = FindFirstFile( path, &find ) ) != INVALID_HANDLE_VALUE )
	{
		do{
			if( _IsNormalFile( &find ) )
			{
				sprintf( path, "%s\\%s", path_dir, find.cFileName );
				DeleteFile( path );
			}
		} while( FindNextFile ( hFind, &find ) );

		FindClose( hFind );
	}
}


bool FileWork_MoveFile( const char* path_src, const char* path_dst, bool b_force )
{
	if( !MoveFile( path_src, path_dst ) )
	{
		if( !CopyFile( path_src, path_dst, false ) ) return false;
		if( b_force ) SetFileAttributes( path_src, FILE_ATTRIBUTE_NORMAL );
		DeleteFile( path_src );
	}
	return true;
}

bool FileWork_ForceDelete( const char* path_src )
{
	SetFileAttributes( path_src, FILE_ATTRIBUTE_NORMAL );
	return DeleteFile( path_src ) ? true : false;
}
