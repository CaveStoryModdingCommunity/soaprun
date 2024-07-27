#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
#include "../../DataTable/dt_MapData.h"

#include "SurfaceNo.h"
#include "Interface.h"

extern char g_module_path[];

RECT _rcNum[] = {
	{  0, 96,  8,104},
	{  8, 96, 16,104},
	{ 16, 96, 24,104},
	{ 24, 96, 32,104},
	{ 32, 96, 40,104},
	{ 40, 96, 48,104},
	{ 48, 96, 56,104},
	{ 56, 96, 64,104},
	{ 64, 96, 72,104},
	{ 72, 96, 80,104},
};

#define PUTNUMFLAG_RIGHT 0x10

// 数字を表示
void PutNumber6( RECT *rcView, int x, int y, int value, unsigned int flag )
{
	RECT rcMinus  = {80,96,88,104};
	int tbl0[]   = {1,10,100,1000,10000,100000};
	int tbl9[]   = {9,99,999,9999,99999,999999};

	int a;
	BOOL bPut   = FALSE;
	BOOL bMinus = FALSE;
	int offset =     0;
	int xpos;


	if( !(flag&0x0F) ) return;

	if( value > tbl9[(flag&0x0F)-1] )
		value = tbl9[(flag&0x0F)-1];
	if( value < 0    ){
		value *= -1;
		bMinus = TRUE;
	}
	
	xpos = x;

	for( offset = (flag&0x0F)-1; offset >= 0; offset-- ){
		a     = value/tbl0[offset];
		value = value%tbl0[offset];
		if( bMinus && a && !bPut ) DxDraw_Put_Clip( rcView, xpos - 8, y, &rcMinus,   SfNo_TENKEY );
		if( bPut || a || !offset ) DxDraw_Put_Clip( rcView, xpos,     y, &_rcNum[a], SfNo_TENKEY );
		if(          a          ) bPut = TRUE;
		xpos += 8;
	}	
}

BOOL if_GenericLoad( HWND hWnd )
{
	BOOL b;

	b = DxDraw_SubSurface_Load( "BMP_TENKEY"   , SfNo_TENKEY   , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_SCROLL_H" , SfNo_SCROLL_H , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_SCROLL_V" , SfNo_SCROLL_V , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_DUMMY"    , SfNo_DUMMY    , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_SYMBOL"   , SfNo_SYMBOL   , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_PALETTE"  , SfNo_PALETTE  , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_TOOLS"    , SfNo_TOOLS    , FALSE, FALSE );
	b = DxDraw_SubSurface_Load( "BMP_UNITTYPE" , SfNo_UNITTYPE , FALSE, FALSE );
//	b = DxDraw_SubSurface_Load( "BMP_DOORTYPE" , SfNo_DOORTYPE , FALSE, FALSE );

//	char path[ MAX_PATH ];
//	sprintf( path, "%s\\data\\unittable.bmp", g_module_path );
//	b = DxDraw_SubSurface_Load( path, SfNo_UNITTABLE, TRUE , FALSE );

//	b = DxDraw_SubSurface_ReadyBlank( MAPPARTS_IMAGE_W, MAPPARTS_IMAGE_H, SfNo_MAPPARTS , TRUE );
//	b = DxDraw_SubSurface_ReadyBlank( MAPPARTS_BMP_W, MAPPARTS_BMP_H, SfNo_UNITTABLE, TRUE );

	return TRUE;
}


// 描画範囲に画像を並べる（水平方向）
void Tiling_Horizontal( const RECT *p_field_rect, const RECT *p_image_rect, int rect_num, int offset, int surf )
{
	int x, y;
	int index;
	int view_width;
	int image_width;

	view_width  = p_field_rect->right   - p_field_rect->left;
	image_width = p_image_rect[0].right - p_image_rect[0].left;

	if( !image_width ) return;

	index       = ( offset / image_width ) % rect_num;

	x = p_field_rect->left - (offset % image_width);
	y = p_field_rect->top;

	while( x <= p_field_rect->right )
	{
		DxDraw_Put_Clip( p_field_rect, x, p_field_rect->top, &p_image_rect[index], surf );
		x += image_width;
		index = (index+1) %rect_num;
	}

}

// 描画範囲に画像を並べる（垂直方向）
void Tiling_Vertical( const RECT *p_field_rect, const RECT *p_image_rect, int rect_num, int offset, int surf )
{
	int x, y;
	int index;
	int view_height;
	int image_height;

	view_height  = p_field_rect->bottom   - p_field_rect->top;
	image_height = p_image_rect[0].bottom - p_image_rect[0].top;

	if( !image_height ) return;

	index       = ( offset / image_height ) % rect_num;

	x = p_field_rect->left;
	y = p_field_rect->top - (offset % image_height);

	while( y <= p_field_rect->bottom )
	{
		DxDraw_Put_Clip( p_field_rect, x, y, &p_image_rect[index], surf );
		y += image_height;
		index = (index+1) %rect_num;
	}

}


