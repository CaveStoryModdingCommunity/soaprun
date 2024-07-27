#include <StdAfx.h>


#include "../DirectX5/ddraw.h"

#include "DxDraw.h"

#define _MAX_SURFACE		64

// 実行ファイルのパス
static char _image_directory[MAX_PATH];

#define _SURFACEFLAG_EXTERIOR 0x01
#define _SURFACEFLAG_SYSTEM   0x02

typedef struct
{
	long                flags   ;
	char*               p_name  ;
	LPDIRECTDRAWSURFACE p_dxsurf;
	short               buf_w   ; // サーフェスのサイズ
	short               buf_h   ;
	short               img_w   ; // イメージ  のサイズ
	short               img_h   ;

}_SURFACESTRUCT;

static _SURFACESTRUCT _surface[_MAX_SURFACE];

// DirectDraw構造体
static LPDIRECTDRAW        _DD      = NULL;  // DirectDraw
static LPDIRECTDRAWSURFACE _surf_F  = NULL;  // 表バッファ
static LPDIRECTDRAWSURFACE _surf_B  = NULL;  // 裏バッファ
static LPDIRECTDRAWCLIPPER _Clipper = NULL;  // クリッパー


// フリップ用変数
static HWND _hWnd        = NULL;
static long _ViewOffsetX = 0;
static long _ViewOffsetY = 0;
static long _ViewWidth   = 0;
static long _ViewHeight  = 0;
static long _TotalPixel  = 0;
static BOOL _bRandom     = FALSE;

static long _mag         = 1;
static BOOL _bFullScreen = FALSE;

static long _RestoreAllSurface( void );

void DxDraw_MainSurface_Release(   void );

static BOOL _malloc_zero( void **pp, long size )
{
	*pp = malloc( size ); if( !( *pp ) ) return FALSE;
	memset( *pp, 0, size );              return TRUE;
}

static void _free( void **pp )
{
	if( *pp ){ free( *pp ); *pp = NULL; }
}

typedef struct
{
	long  size    ;
	long* p_table ;
	long  buf[ 2 ];
}
_RANDOMTABLESTRUCT;

static short _random_get( _RANDOMTABLESTRUCT* p_rand )
{
	long w1, w2;
	char *p1;
	char *p2;

	w1 = (short)( p_rand->buf[ 0 ] + p_rand->buf[ 1 ] );

	p1 = (char *)&w1;
	p2 = (char *)&w2;

	p2[ 0 ] = p1[ 1 ];
	p2[ 1 ] = p1[ 0 ];

	p_rand->buf[ 1 ] = (short)p_rand->buf[ 0 ];
	p_rand->buf[ 0 ] = (short)w2;

	return (short)w2;
}

static BOOL _random_make( _RANDOMTABLESTRUCT* p_rand, long size, unsigned short a, unsigned short b )
{
	long step, s;

	memset( p_rand, 0, sizeof(_RANDOMTABLESTRUCT) );

	p_rand->buf[ 0 ] = a   ;
	p_rand->buf[ 1 ] = b   ;
	p_rand->size     = size;

	p_rand->p_table = (long *)malloc( sizeof(long) * size ); if( !p_rand->p_table ) return FALSE;

	memset( p_rand->p_table, 0xff, sizeof(long) * size );

	s = 0;
	for( long i = 0; i < size; i++ )
	{
		step = ( (unsigned short)_random_get( p_rand ) >> 8 );
		if( !step ) step = 1;
		while( 1 )
		{
			s++;
			s = s % size;
			if( p_rand->p_table[ s ] == -1 ) step--;
			if( !step ) break; 
		}
		p_rand->p_table[ s ] = i;
	}

	return TRUE;
}

static void _random_release( _RANDOMTABLESTRUCT* p_rand )
{
	if( p_rand->p_table ){ free( p_rand->p_table ); p_rand->p_table = NULL; }
}


/*
#define _MAX_DEVMODE 100

static BOOL _SetFullScreen( HWND hWnd )
{
	DEVMODE mode;
	DWORD   min_b = 0, min_w, min_h;
	BOOL    bOk = FALSE;

	// 最小サイズ
	for( long i = 0; i < _MAX_DEVMODE; i++ )
	{
		if( !EnumDisplaySettings( NULL, i, &mode ) ) break;
		if( !i || mode.dmPelsWidth < min_w )
		{
			min_w = mode.dmPelsWidth ;
			min_h = mode.dmPelsHeight;
			min_b = mode.dmBitsPerPel;
			bOk      = TRUE;
		}
	}
	if( !min_b ) return FALSE;

	// 最小 bit / sample
	for( long i = 0; i < _MAX_DEVMODE; i++ )
	{
		if( !EnumDisplaySettings( NULL, i, &mode ) ) break;
		if( mode.dmPelsWidth == min_w && mode.dmPelsHeight == min_h )
		{
			if( mode.dmBitsPerPel < min_b ) min_b = mode.dmBitsPerPel;
		}
	}
	MessageBox( 0, "0.7", "", 0 );

	if( DD_OK != _DD->SetCooperativeLevel( hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE ) ) return FALSE;
	MessageBox( 0, "0.8", "", 0 );
	if( DD_OK != _DD->SetDisplayMode( min_w, min_h, min_b ) ) return FALSE;
	MessageBox( 0, "0.9", "", 0 );

	return TRUE;
}
*/


// サブサーフェスの解放
void DxDraw_SubSurface_Release( long s )
{
	_SURFACESTRUCT *p_surf;

	if( s >= _MAX_SURFACE ) return;

	p_surf = &_surface[ s ];

	if( p_surf->p_dxsurf )
	{
		_free( (void**)&p_surf->p_name );
		p_surf->p_dxsurf->Release();
		_TotalPixel -= p_surf->buf_w * _mag * p_surf->buf_h * _mag;
		memset( p_surf, 0, sizeof(_SURFACESTRUCT) );
	}
}

// 全てのサブサーフェスの解放
void DxDraw_SubSurface_ReleaseAll( void )
{
	long s;
	for( s = 0; s < _MAX_SURFACE; s++ ) DxDraw_SubSurface_Release( s );
}


/////////////////////////////
// ローカル関数 /////////////
/////////////////////////////


static BOOL _CreateSurface( LPDIRECTDRAWSURFACE *pp_dxsurf, long width, long height, BOOL bSystemMemory )
{
	// パターン用バッファの生成
	DDSURFACEDESC ddSD;
    ZeroMemory( &ddSD, sizeof(DDSURFACEDESC) );
    ddSD.dwSize      = sizeof(DDSURFACEDESC);
    ddSD.dwFlags     = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddSD.dwWidth     = width  * _mag;  //幅
	ddSD.dwHeight    = height * _mag; //高さ
	if( bSystemMemory ) ddSD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
	else                ddSD.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	if( DD_OK != _DD->CreateSurface( &ddSD, pp_dxsurf, NULL ) ) return FALSE;

	_TotalPixel += ddSD.dwWidth * ddSD.dwHeight;

	// カラーキーの設定
    DDCOLORKEY ddColorKey;
    ddColorKey.dwColorSpaceLowValue  = 0;
    ddColorKey.dwColorSpaceHighValue = 0;
    (*pp_dxsurf)->SetColorKey( DDCKEY_SRCBLT, &ddColorKey );

//　　DDCOLORKEY ddck;
//　　ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue = DDColorMatch(*lpBITMAP,RGB(0, 0xff, 0));
//　　(*pp_dxsurf)->SetColorKey( DDCKEY_SRCBLT, &ddck );


	return TRUE;

}

static void _BltImageToSurface( LPDIRECTDRAWSURFACE p_dxsurf, const HBITMAP hBmp, long bmp_w, long bmp_h )
{
	HBITMAP hBmpOld;
	HDC     hDCs;
	HDC     hDCd;

	hDCs    = CreateCompatibleDC( NULL ); 
	hBmpOld = (HBITMAP)SelectObject( hDCs, hBmp );
	p_dxsurf->GetDC( &hDCd );


	if( _bRandom )
	{

		_RANDOMTABLESTRUCT rand;
		if( _random_make( &rand, bmp_h, 0x4444, 0x8888 ) )
		{
			for( long y = 0; y < bmp_h; y++ )
			{
				StretchBlt( hDCd, 0, y * _mag         , bmp_w * _mag, 1 * _mag, hDCs,
								  0, rand.p_table[ y ], bmp_w       , 1       , SRCCOPY );
			}
			_random_release( &rand );
		}
	}
	else
	{
//		BitBlt(     hDCd, 0, 0, bmp_w * _mag, bmp_h * _mag, hDCs, 0, 0, SRCCOPY );
		StretchBlt( hDCd, 0, 0, bmp_w * _mag, bmp_h * _mag,	hDCs, 0, 0, bmp_w, bmp_h, SRCCOPY );
	}

	p_dxsurf->ReleaseDC( hDCd );
	SelectObject( hDCs, hBmpOld );
	DeleteDC(     hDCs );


	{
		DDSURFACEDESC ddsd;
		unsigned long color;

		// バックバッファにアクセスして最初のPixelを取得
		memset( &ddsd, 0, sizeof(DDSURFACEDESC) );
		ddsd.dwSize =     sizeof(DDSURFACEDESC);

		if( DD_OK != p_dxsurf->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) ) return;
		color = *(DWORD *)ddsd.lpSurface;
		if( ddsd.ddpfPixelFormat.dwRGBBitCount < sizeof(DWORD) * 8 ) color &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;

		p_dxsurf->Unlock( NULL );

		DDCOLORKEY ddck;
		ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue = color;
		p_dxsurf->SetColorKey( DDCKEY_SRCBLT, &ddck );
	}


}

/////////////////////////////
// グローバル関数 ///////////
/////////////////////////////

long DxDraw_GetTotalPixel( void ){ return _TotalPixel; }


// フリップで使う オフセット
void DxDraw_SetViewOffset( long offset_x, long offset_y )
{
	_ViewOffsetX  = offset_x;
	_ViewOffsetY  = offset_y;
}

void DxDraw_SetViewOffset_Center( void )
{
	_ViewOffsetX = ( GetSystemMetrics( SM_CXSCREEN ) - _ViewWidth  * _mag ) / 2;
	_ViewOffsetY = ( GetSystemMetrics( SM_CYSCREEN ) - _ViewHeight * _mag ) / 2;
}

BOOL DxDraw_CooperativeLevel( BOOL bFullScreen )
{
	if( !_DD ) return FALSE;

	if( bFullScreen )
	{
		if( _DD->SetCooperativeLevel( _hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE ) != DD_OK ) return FALSE;
	}
	else
	{
		if( _DD->SetCooperativeLevel( _hWnd, DDSCL_NORMAL )                       != DD_OK ) return FALSE;
		_ViewOffsetX = 0;
		_ViewOffsetY = 0;
	}

	_bFullScreen = bFullScreen;

	return TRUE;
}



BOOL DxDraw_Initialize( HWND hWnd, const char *image_directory, BOOL bFullScreen, BOOL bRandom )
{
	// 画像用のディレクトリを設定
	memset( _image_directory, 0, MAX_PATH );
	if( image_directory ) strcpy( _image_directory, image_directory );
	memset( _surface, 0, sizeof(_SURFACESTRUCT) * _MAX_SURFACE );
	if( DirectDrawCreate( NULL, &_DD, NULL ) != DD_OK ) return FALSE;
	_hWnd = hWnd;
	if( !DxDraw_CooperativeLevel( bFullScreen )       ) return FALSE;

	_TotalPixel = 0;
	_bRandom    = bRandom;

	return TRUE;
}

// DirectDrawの終了
void DxDraw_Release( HWND hWnd )
{
	if( !_DD ) return;
	DxDraw_SubSurface_ReleaseAll();
	DxDraw_MainSurface_Release();
	_DD->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
	_DD->Release();
	_DD = NULL;
	_hWnd = NULL;
}



BOOL DxDraw_MainSurface_Ready( HWND hWnd, long width, long height, long mag )
{
	// ビューの幅と高さを保存
	_ViewWidth  = width ;
	_ViewHeight = height;
	_mag        = mag   ;

	// 表バッファの初期化
	DDSURFACEDESC ddsDesc;
	memset( &ddsDesc, 0, sizeof(DDSURFACEDESC) );
	ddsDesc.dwSize            = sizeof(ddsDesc);

	ddsDesc.dwFlags           = DDSD_CAPS;
	ddsDesc.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;
	ddsDesc.dwBackBufferCount = 0;

	if( DD_OK != _DD->CreateSurface(&ddsDesc, &_surf_F, NULL) ) return FALSE;

	// 裏バッファの生成
    memset( &ddsDesc, 0, sizeof(DDSURFACEDESC) );
    ddsDesc.dwSize         = sizeof(ddsDesc);
    ddsDesc.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsDesc.dwWidth        = _ViewWidth  * _mag;
	ddsDesc.dwHeight       = _ViewHeight * _mag;
	if( DD_OK != _DD->CreateSurface( &ddsDesc, &_surf_B, NULL) ) return FALSE;
	
	//クリッパーの作成
	_DD->CreateClipper( NULL , &_Clipper , NULL );
	_Clipper->SetHWnd( 0 , hWnd );
	_surf_F->SetClipper( _Clipper );

	// クリア
	DDBLTFX ddBltFX;
	RECT rect;

	ZeroMemory( &ddBltFX, sizeof(DDBLTFX) );
	ddBltFX.dwSize      = sizeof(DDBLTFX);
	ddBltFX.dwFillColor = 0;
	rect.left           = 0;
	rect.right          = _ViewWidth  * _mag;
	rect.top            = 0;
	rect.bottom         = _ViewHeight * _mag;
	_surf_B->Blt( &rect, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );

	_TotalPixel += _ViewWidth * _mag * _ViewHeight * _mag;
	return TRUE;
}

// メインサーフェスの開放
void DxDraw_MainSurface_Release( void )
{
    if( _surf_F )
	{
		_surf_F->Release();
		_surf_F = NULL;
		_TotalPixel -= _ViewWidth * _mag * _ViewHeight * _mag;
	}
}

// フリップ
BOOL DxDraw_Flip_Fixed( void )
{
	RECT rcTo;
	RECT rcFrom;

	if( !_DD || !_hWnd ) return FALSE;

	// 転送元
	rcFrom.left   = 0;
	rcFrom.top    = 0;
	rcFrom.right  = _ViewWidth  * _mag;
	rcFrom.bottom = _ViewHeight * _mag;

	// 転送先
	GetWindowRect( _hWnd, &rcTo );
	rcTo.left   += _ViewOffsetX; 
	rcTo.top    += _ViewOffsetY;
	rcTo.right  = rcTo.left + _ViewWidth * _mag;
	rcTo.bottom = rcTo.top  + _ViewHeight * _mag;

	// バックバッファをフロントバッファへコピー
	_surf_F->Blt( &rcTo, _surf_B, &rcFrom, DDBLT_WAIT, 0 );

	if( _RestoreAllSurface() ){ }
	return TRUE;
}


/////////////////////////////
// サーフェスの生成 /////////
/////////////////////////////

// サーフェスを作ってビットマップを読み込む(リソースから)
BOOL DxDraw_SubSurface_Load( const char *name, long s, BOOL bExterior, BOOL bSystem )
{
    HBITMAP        hBmp;
	BITMAP         bmp;
	char           path[MAX_PATH];
	int            load_flag;
	_SURFACESTRUCT *p_surf;

	if( s >= _MAX_SURFACE      ) return FALSE;
	if( _surface[ s ].p_dxsurf ) return FALSE;

	p_surf = &_surface[ s ];

	// 外部ファイル
	if( bExterior ){
		if( name[ 1 ] != ':' ) sprintf( path, "%s\\%s", _image_directory, name );
		else                   strcpy(  path, name );
		load_flag = LR_LOADFROMFILE|LR_CREATEDIBSECTION;

	// リソース
	}else{
		strcpy( path, name );
		load_flag = LR_CREATEDIBSECTION;
	}

	// 読み込み
	hBmp = (HBITMAP)LoadImage( GetModuleHandle(NULL), path, IMAGE_BITMAP, 0, 0, load_flag );
    if( !hBmp ) return FALSE;
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	if( !_CreateSurface( &p_surf->p_dxsurf, bmp.bmWidth, bmp.bmHeight, bSystem ) ) return FALSE;
	_BltImageToSurface(   p_surf->p_dxsurf, hBmp, bmp.bmWidth, bmp.bmHeight );
	DeleteObject( hBmp );

	// 情報を保持
	_free(             (void**)&p_surf->p_name );
	if( !_malloc_zero( (void**)&p_surf->p_name, strlen( name ) + 1 ) ) DxDraw_SubSurface_Release( s );
	strcpy( p_surf->p_name, name );
	p_surf->img_w = p_surf->buf_w = (short)bmp.bmWidth ;
	p_surf->img_h = p_surf->buf_h = (short)bmp.bmHeight;
	p_surf->flags = 0;
	if( bExterior ) p_surf->flags |= _SURFACEFLAG_EXTERIOR;
	if( bSystem   ) p_surf->flags |= _SURFACEFLAG_SYSTEM;

    return TRUE;
}


// 空っぽサーフェスを生成
BOOL DxDraw_SubSurface_ReadyBlank( long width, long height, long s, BOOL bSystem )
{
	_SURFACESTRUCT *p_surf;

	if( s >= _MAX_SURFACE      ) return FALSE;
	if( _surface[ s ].p_dxsurf ) return FALSE;
	p_surf = &_surface[ s ];

	if( !_CreateSurface( &p_surf->p_dxsurf, width, height, bSystem ) ) return FALSE;

	// 情報を保持
	_free( (void**)&p_surf->p_name );

	p_surf->img_w = p_surf->buf_w = (short)width ;
	p_surf->img_h = p_surf->buf_h = (short)height;

	if( bSystem ) p_surf->flags |=  _SURFACEFLAG_SYSTEM;
	else          p_surf->flags &= ~_SURFACEFLAG_SYSTEM;

	// 使用Pixelを計算
	return TRUE;
}

//既存のサーフェスにビットマップを読み込む
BOOL DxDraw_SubSurface_Reload( const char *name, long s, BOOL bExterior )
{
    HBITMAP        hBmp;
	BITMAP         bmp;
	char           path[MAX_PATH];
	int            load_flag;
	_SURFACESTRUCT *p_surf;

	if( s >= _MAX_SURFACE       ) return FALSE;
	if( !_surface[ s ].p_dxsurf ) return FALSE;

	p_surf = &_surface[ s ];

	// 外部ファイル
	if( bExterior )
	{
		if( name[ 1 ] != ':' ) sprintf( path, "%s\\%s", _image_directory, name );
		else                   strcpy(  path, name );
		load_flag = LR_LOADFROMFILE|LR_CREATEDIBSECTION;

	// リソース
	}else{
		strcpy(  path, name );
		load_flag = LR_CREATEDIBSECTION;
	}

	// 読み込み
	hBmp = (HBITMAP)LoadImage( GetModuleHandle(NULL), path, IMAGE_BITMAP, 0, 0, load_flag );
	if( !hBmp ) return  FALSE;
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	_BltImageToSurface( p_surf->p_dxsurf, hBmp, bmp.bmWidth, bmp.bmHeight );
	DeleteObject( hBmp );

	// 情報を保持
	_free(             (void**)&p_surf->p_name );
	if( !_malloc_zero( (void**)&p_surf->p_name, strlen( name ) + 1 ) ) DxDraw_SubSurface_Release( s );
	strcpy( p_surf->p_name, name );

	p_surf->img_w = (short)bmp.bmWidth ;
	p_surf->img_h = (short)bmp.bmHeight;

	if( bExterior ) p_surf->flags |=  _SURFACEFLAG_EXTERIOR;
	else            p_surf->flags &= ~_SURFACEFLAG_EXTERIOR;

	return TRUE;
}



BOOL DxDraw_SubSurface_GetImageSize( long *p_w, long *p_h, long s )
{
	if( s >= _MAX_SURFACE       ) return FALSE;
	if( !_surface[ s ].p_dxsurf ) return FALSE;
	if( p_w ) *p_w = _surface[ s ].img_w;
	if( p_h ) *p_h = _surface[ s ].img_h;
	return TRUE;
}


/////////////////////////////
// イメージ描画 /////////////
/////////////////////////////

void _MagRect( RECT *p_rect )
{
	p_rect->left   *= _mag;
	p_rect->right  *= _mag;
	p_rect->top    *= _mag;
	p_rect->bottom *= _mag;
}

//裏バッファに描画
void DxDraw_Put( long x, long y, const RECT *p_rect, long s )
{
	RECT rcTo;
	RECT rcFrom;

	if( s >= _MAX_SURFACE       ) return;
	if( !_surface[ s ].p_dxsurf ) return;

	rcFrom      = *p_rect;
	rcTo.left   = x;
	rcTo.right  = x + p_rect->right  - p_rect->left;
	rcTo.top    = y;
	rcTo.bottom = y + p_rect->bottom - p_rect->top;

	_MagRect( &rcFrom );
	_MagRect( &rcTo   );
	_surf_B->Blt( &rcTo, _surface[ s ].p_dxsurf, &rcFrom, DDBLT_WAIT|DDBLT_KEYSRC, 0 );
}

// 裏バッファをサーフェスにコピー
void DxDraw_CopyBackBuffer( long s )
{
	DDBLTFX ddBltFX;
	RECT    rect;

	if( s >= _MAX_SURFACE       ) return;
	if( !_surface[ s ].p_dxsurf ) return;

	ZeroMemory( &ddBltFX, sizeof(DDBLTFX) );
	ddBltFX.dwSize = sizeof( DDBLTFX );
	rect.left   = 0;
	rect.right  = _ViewWidth;
	rect.top    = 0;
	rect.bottom = _ViewHeight;

	_MagRect( &rect );
	_surface[ s ].p_dxsurf->Blt( &rect, _surf_B, &rect, DDBLT_WAIT, &ddBltFX );
}


// 裏バッファに描画（表示領域指定付き 自前クリップ）
void DxDraw_Put_Clip( const RECT *rcView, long x, long y, const RECT *p_rc, long s )
{

	RECT rcFrom;
	RECT rcTo;

	if( s >= _MAX_SURFACE       ) return;
	if( !_surface[ s ].p_dxsurf ) return;

	rcFrom = *p_rc;

	// 右が出たら
	if( x + p_rc->right - p_rc->left > rcView->right ){
		rcFrom.right  -= ( x + p_rc->right - p_rc->left ) - rcView->right ;
	}

	// 左が出たら
	if( x < rcView->left){
		rcFrom.left += rcView->left - x;
		x = rcView->left;
	}

	// 下が出たら
	if( y + p_rc->bottom - p_rc->top > rcView->bottom ){
		rcFrom.bottom -= ( y + p_rc->bottom - p_rc->top) - rcView->bottom;
	}

	// 上が出たら
	if( y < rcView->top ){
		rcFrom.top += rcView->top - y;
		y = rcView->top;
	}

	rcTo.left   = x;
	rcTo.top    = y;
	rcTo.right  = x + rcFrom.right  - rcFrom.left;
	rcTo.bottom = y + rcFrom.bottom - rcFrom.top;

	_MagRect( &rcFrom );
	_MagRect( &rcTo   );
	_surf_B->Blt( &rcTo, _surface[ s ].p_dxsurf, &rcFrom, DDBLT_WAIT|DDBLT_KEYSRC, 0 );

}

// サーフェスからサーフェスへコピー
void DxDraw_CopySurface( long x, long y, const RECT *p_rect, long to, long from )
{
	RECT rcTo;
	RECT rcFrom;

	if( to   >= _MAX_SURFACE      ) return;
	if( from >= _MAX_SURFACE      ) return;
	if( !_surface[ to   ].p_dxsurf ) return;
	if( !_surface[ from ].p_dxsurf ) return;

	rcFrom = *p_rect;

	rcTo.left   = x;
	rcTo.top    = y;
	rcTo.right  = x + p_rect->right  - p_rect->left;
	rcTo.bottom = y + p_rect->bottom - p_rect->top;

	_MagRect( &rcFrom );
	_MagRect( &rcTo   );
	_surface[to].p_dxsurf->Blt( &rcTo, _surface[from].p_dxsurf, &rcFrom, DDBLT_WAIT|DDBLT_KEYSRC, 0 );
}

/////////////////////////////
// 矩形描画 /////////////////
/////////////////////////////

// 裏バッファに矩形を描画
void DxDraw_PaintRect( const RECT *p_rect, unsigned long col )
{
	DDBLTFX ddBltFX;
	RECT rect;

	ZeroMemory( &ddBltFX, sizeof(DDBLTFX) );
	ddBltFX.dwSize      = sizeof(DDBLTFX);
	ddBltFX.dwFillColor = col;
	rect                = *p_rect;

	_MagRect( &rect );
	_surf_B->Blt( &rect, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
}

// 裏バッファに矩形を描画
void DxDraw_PaintRect_Clip( const RECT *rc_view, const RECT *p_rect, unsigned long col )
{
	DDBLTFX ddBltFX;
	RECT rect;

	ZeroMemory( &ddBltFX, sizeof(DDBLTFX) );
	ddBltFX.dwSize      = sizeof(DDBLTFX);
	ddBltFX.dwFillColor = col;
	rect                = *p_rect;

	if( rect.left   < rc_view->left   ) rect.left   = rc_view->left  ;
	if( rect.top    < rc_view->top    ) rect.top    = rc_view->top   ;
	if( rect.right  > rc_view->right  ) rect.right  = rc_view->right ;
	if( rect.bottom > rc_view->bottom ) rect.bottom = rc_view->bottom;

	_MagRect( &rect );
	_surf_B->Blt( &rect, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
}

// 裏バッファに枠を描画
void DxDraw_PaintFrame( const RECT *rc_view, const RECT *p_rect, unsigned long col )
{
	DDBLTFX ddBltFX;
	RECT rc1, rc2;

	ZeroMemory( &ddBltFX, sizeof(DDBLTFX) );
	ddBltFX.dwSize      = sizeof(DDBLTFX);
	ddBltFX.dwFillColor = col;
	rc1                 = *p_rect;

	if( rc1.left   < rc_view->left   ) rc1.left   = rc_view->left  ;
	if( rc1.top    < rc_view->top    ) rc1.top    = rc_view->top   ;
	if( rc1.right  > rc_view->right  ) rc1.right  = rc_view->right ;
	if( rc1.bottom > rc_view->bottom ) rc1.bottom = rc_view->bottom;

	rc2 = rc1; rc2.right  = rc2.left   + 1; _MagRect( &rc2 ); _surf_B->Blt( &rc2, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
	rc2 = rc1; rc2.bottom = rc2.top    + 1; _MagRect( &rc2 ); _surf_B->Blt( &rc2, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
	rc2 = rc1; rc2.left   = rc2.right  - 1; _MagRect( &rc2 ); _surf_B->Blt( &rc2, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
	rc2 = rc1; rc2.top    = rc2.bottom - 1; _MagRect( &rc2 ); _surf_B->Blt( &rc2, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
}

// サーフェスに矩形を描画
void DxDraw_PaintRect_Surface( const RECT *p_rect, unsigned long col, long s )
{
	DDBLTFX ddBltFX;
	RECT    rect;

	if( s >= _MAX_SURFACE      ) return;
	if( !_surface[ s ].p_dxsurf ) return;

	ZeroMemory( &ddBltFX, sizeof(DDBLTFX) );
	ddBltFX.dwSize      = sizeof(DDBLTFX);
	ddBltFX.dwFillColor = col;
	if( p_rect )
	{
		rect   = *p_rect;
	}
	else
	{
		rect.left   = 0; rect.right  = _surface[ s ].buf_w;
		rect.top    = 0; rect.bottom = _surface[ s ].buf_h;
	}

	_MagRect( &rect );
	_surface[ s ].p_dxsurf->Blt( &rect, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddBltFX );
}




// CortBox用の色を取得
unsigned long DxDraw_GetPaintColor( COLORREF rgb )
{
	HDC hDC;
	DDSURFACEDESC ddsd;

	unsigned long color;
	COLORREF temp_rgb;
	

	// バックバッファの最初のPixelを保存して
	// 指定の色をつける
	if( DD_OK != _surf_B->GetDC( &hDC ) )
		return CLR_INVALID;
	temp_rgb = GetPixel( hDC, 0, 0 );
	SetPixel( hDC, 0, 0, rgb );
	_surf_B->ReleaseDC( hDC );

	// バックバッファにアクセスして最初のPixelを取得
	memset( &ddsd, 0, sizeof(DDSURFACEDESC) );
	ddsd.dwSize =     sizeof(DDSURFACEDESC);

	if( DD_OK != _surf_B->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) )
		return CLR_INVALID;
	color = *(DWORD *)ddsd.lpSurface;
	if( ddsd.ddpfPixelFormat.dwRGBBitCount < sizeof(DWORD) * 8 ) 
		color &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;

	_surf_B->Unlock( NULL );

	// 最初に置かれていた色を配置
	if( DD_OK != _surf_B->GetDC( &hDC ) )
		return CLR_INVALID;
	SetPixel( hDC, 0, 0, temp_rgb );
	_surf_B->ReleaseDC( hDC );

	return color;
}

unsigned long DxDraw_GetSurfaceColor( long s )
{
	DDSURFACEDESC ddsd;
	unsigned long color;

	// バックバッファにアクセスして最初のPixelを取得
	memset( &ddsd, 0, sizeof(DDSURFACEDESC) );
	ddsd.dwSize =     sizeof(DDSURFACEDESC);

	if( DD_OK != _surface[ s ].p_dxsurf->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) ) return CLR_INVALID;
	color = *(DWORD *)ddsd.lpSurface;
	if( ddsd.ddpfPixelFormat.dwRGBBitCount < sizeof(DWORD) * 8 ) color &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;

	_surface[ s ].p_dxsurf->Unlock( NULL );

	return color;
}

/////////////////////////////
// 文字列 ///////////////////
/////////////////////////////

static HFONT ghFont = NULL;

//初期化時に作成
void DxDraw_Text_Ready( const char *font_name, char x_size, long y_size )
{
	ghFont = CreateFont(
		y_size, x_size, // サイズ
		0,              // 角度
		0,
		FW_NORMAL,
		FALSE,          // Italic
		FALSE,          // Underline
		FALSE,          // StrikeOut
		DEFAULT_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FIXED_PITCH,
		font_name );
}

// 裏バッファに文字列を描画
void DxDraw_Text_Put( long x, long y, const char *str, COLORREF col )
{
	HFONT oldFont;
	HDC   hDC;

	_surf_B->GetDC( &hDC );
	oldFont = (HFONT)SelectObject( hDC, ghFont );
	SetBkMode( hDC, TRANSPARENT );           //背景色を透過
	SetTextColor( hDC, col );             //カラー設定

	TextOut( hDC, x*_mag, y*_mag, str, strlen( str ) );//描画

	SelectObject( hDC, oldFont );
	_surf_B->ReleaseDC( hDC );
}

// サーフェスにに文字列を描画
void DxDraw_Text_Put_Surface( long x, long y, const char *str, COLORREF col, long s )
{
	HFONT   oldFont;
	HDC     hDC;

	if( s >= _MAX_SURFACE       ) return;
	if( !_surface[ s ].p_dxsurf ) return;

	_surface[ s ].p_dxsurf->GetDC( &hDC );
	oldFont = (HFONT)SelectObject( hDC, ghFont );
	SetBkMode( hDC, TRANSPARENT );           //背景色を透過
	SetTextColor( hDC, col );                //カラー設定

	TextOut( hDC, x*_mag, y*_mag, str, strlen( str ) );//描画

	SelectObject( hDC,oldFont );
	_surface[ s ].p_dxsurf->ReleaseDC( hDC );
}



//終了時に削除
void DxDraw_Text_Release( void )
{
	DeleteObject( ghFont );
}


/////////////////////////////
// サーフェースの復帰 ///////
/////////////////////////////

// 吐き出す
BOOL _out( unsigned char c )
{
	return TRUE;

	FILE *fp;
	char path[MAX_PATH];

	sprintf( path, "%s\\out.txt", _image_directory );

	fp = fopen( path, "a+t" );
	if( !fp )return FALSE;

	fprintf( fp, "%c", c );
	fclose( fp );
	return TRUE;
}


// 失ったサーフェスを復帰する
static long _RestoreAllSurface( void )
{
	long           num = 0;
	char           name[ MAX_PATH ];
	BOOL           bExterior;
	RECT           rc = {0,0,0,0};
	_SURFACESTRUCT *p_surf;

	if( !_surf_F ) return num;
	if( !_surf_B ) return num;

	if( _surf_F->IsLost() == DDERR_SURFACELOST ){
		 num++;
		_surf_F->Restore();
		_out( 'f' );
	}
	if( _surf_B->IsLost() == DDERR_SURFACELOST ){
		 num++;
		_surf_B->Restore();
		_out( 'b' );
	}

	for( long s = 0; s < _MAX_SURFACE; s++ )
	{
		if( _surface[ s ].p_dxsurf )
		{
			p_surf = &_surface[ s ];
			if( p_surf->p_dxsurf->IsLost() == DDERR_SURFACELOST )
			{
				 num++;
				p_surf->p_dxsurf->Restore();
				_out( (unsigned char)(s + '0') );

				if( !( p_surf->flags & _SURFACEFLAG_SYSTEM ) )
				{
					if( p_surf->p_name )
					{
						strcpy( name, p_surf->p_name );
						if( p_surf->flags & _SURFACEFLAG_EXTERIOR ) bExterior = TRUE;
						else                                        bExterior = FALSE;
						DxDraw_SubSurface_Reload( name, s, bExterior );
					}
					else
					{
//						rc.right  = p_surf->x_size;
//						rc.bottom = p_surf->y_size;
//						DxDraw_PaintRect_Surface( &rc, 0, s );
						DxDraw_PaintRect_Surface( NULL, 0, s );
					}

				}
			}
		}
	}
	return num;
}

BOOL DxDraw_ChangeMagnify( HWND hWnd, long mag )
{
	long           s;
	char           path[ MAX_PATH ];
	BOOL           bSystem;
	BOOL           bExterior;
	long           old_mag;
	_SURFACESTRUCT *p_surf;

	if( _mag == mag ) return TRUE;
	old_mag = _mag;

	// メインサーフェス
	DxDraw_MainSurface_Release();
	DxDraw_MainSurface_Ready( hWnd, _ViewWidth, _ViewHeight, mag );

	for( s = 0; s < _MAX_SURFACE; s++ )
	{
		if( _surface[ s ].p_dxsurf )
		{
			p_surf = &_surface[ s ];
			p_surf->p_dxsurf->Release();
			p_surf->p_dxsurf = NULL;
			_TotalPixel -= p_surf->buf_w * old_mag * p_surf->buf_h * old_mag;
			if( p_surf->flags & _SURFACEFLAG_EXTERIOR ) bExterior = TRUE ;
			else                                        bExterior = FALSE;
			if( p_surf->flags & _SURFACEFLAG_SYSTEM   ) bSystem   = TRUE ;
			else                                        bSystem   = FALSE;
			if( p_surf->p_name )
			{
				strcpy( path, p_surf->p_name );
				DxDraw_SubSurface_ReadyBlank( p_surf->buf_w, p_surf->buf_h, s, bSystem );
				DxDraw_SubSurface_Reload( path, s, bExterior );
			}
			else
			{
				DxDraw_SubSurface_ReadyBlank( p_surf->buf_w, p_surf->buf_h, s, bSystem );
			}
		}
	}

	return TRUE;
}

