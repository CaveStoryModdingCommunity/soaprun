#pragma once

typedef void (* DXDRAWFUNCTION)( void );

long DxDraw_GetTotalPixel( void );
void DxDraw_SetViewOffset( long offset_x, long offset_y );
void DxDraw_SetViewOffset_Center( void );
BOOL DxDraw_Initialize( HWND hWnd, const char *image_directory, BOOL bFullScreen, BOOL bRandom );
BOOL DxDraw_CooperativeLevel( BOOL b );
void DxDraw_Release(       HWND hWnd );
BOOL DxDraw_ChangeMagnify( HWND hWnd, long mag );

// メインサーフェス
BOOL DxDraw_MainSurface_Ready( HWND hWnd, long width, long height, long mag );
void DxDraw_MainSurface_Release( void );
BOOL DxDraw_Flip_Fixed( void );

// サブサーフェス
BOOL DxDraw_SubSurface_Load(   const char *name,    long s, BOOL bExterior, BOOL bSystem );
BOOL DxDraw_SubSurface_ReadyBlank( long width, long height, long s, BOOL bSystem );
BOOL DxDraw_SubSurface_Reload( const char *name,    long s, BOOL bExterior );
void DxDraw_SubSurface_Release(                             long s );
void DxDraw_SubSurface_ReleaseAll( void );
BOOL DxDraw_SubSurface_GetImageSize( long *p_w, long *p_h, long s );

// 画像表示
void DxDraw_Put(                          long x, long y, const RECT *p_rect, long s );
void DxDraw_Put_Clip( const RECT *rcView, long x, long y, const RECT *p_rect, long s );
void DxDraw_CopySurface(                  long x, long y, const RECT *p_rect, long to, long from );
void DxDraw_CopyBackBuffer( long s );

// 矩形表示
unsigned long DxDraw_GetPaintColor( COLORREF rgb );
unsigned long DxDraw_GetSurfaceColor( long s );

void DxDraw_PaintRect(                           const RECT *p_rect, unsigned long col );
void DxDraw_PaintRect_Surface(                   const RECT *p_rect, unsigned long col, long s );
void DxDraw_PaintRect_Clip( const RECT *rc_view, const RECT *p_rect, unsigned long col );
void DxDraw_PaintFrame(     const RECT *rc_view, const RECT *p_rect, unsigned long col );

// 文字表示
void DxDraw_Text_Ready( const char *font_name, char x_size, long y_size );
void DxDraw_Text_Put(         long x, long y, const char *str, COLORREF col );
void DxDraw_Text_Put_Surface( long x, long y, const char *str, COLORREF col, long s );
void DxDraw_Text_Release( void );

