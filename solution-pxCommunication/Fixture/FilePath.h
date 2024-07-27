#pragma once

BOOL FilePath_SelectLoad( HWND hWnd, char *path, const char *title, const char *ext, const char *filter );
BOOL FilePath_SelectSave( HWND hWnd, char *path, const char *title, const char *ext, const char *filter );
BOOL FilePath_LoadLastFolder(       char *last_path, const char *name );
BOOL FilePath_SaveLastFolder( const char *last_path, const char *name );
void FilePath_Fatal_x( char *name );

BOOL FilePath_SelectDirectory( HWND hWnd, const char *title, char *path );
