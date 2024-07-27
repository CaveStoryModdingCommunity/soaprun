#include <StdAfx.h>

#include "../../Dx/DxDraw.h"

#include "FontWork.h"

#define _CHARIMAGE_W     8
#define _CHARIMAGE_H    12
#define _CHARIMAGE_XNUM 32


unsigned short FontWork_GetWidth( unsigned short code )
{
	if(      code >= 0x0020 && code < 0x0080 ) return FONTWORK_FONTW_C;  // 半角
	else if( code >= 0x00A0 && code < 0x00E0 ) return FONTWORK_FONTW_C;  // 半角カナ
	else if( code >= 0x8140 && code < 0x81b0 ) return FONTWORK_FONTW_W;  // 全角記号
	else if( code >= 0x824F && code < 0x82f7 ) return FONTWORK_FONTW_W;  // 全角（数字・ABC・あいう）
	else if( code >= 0x8340 && code < 0x8398 ) return FONTWORK_FONTW_W;  // 全角（カタカナ）
	else if( code == 0x897e )                  return FONTWORK_FONTW_W;
	return 0;
}

void FontWork_Put( const RECT* p_rc_view, int x, int y, unsigned short code, int flags )
{
	long index;

	if(      code >= 0x0020 && code < 0x0080 ) index = code - 0x0020 + 0x0000; // 半角
	else if( code >= 0x00A0 && code < 0x00E0 ) index = code - 0x00A0 + 0x0060; // 半角カナ
	else if( code >= 0x8140 && code < 0x81b0 ) index = code - 0x8140 + 0x00A0; // 全角記号
	else if( code >= 0x824F && code < 0x82f7 ) index = code - 0x824F + 0x0110; // 全角（数字・ABC・あいう）
	else if( code >= 0x8340 && code < 0x8398 ) index = code - 0x8340 + 0x01C0; // 全角（カタカナ）
	else if( code == 0x897e                  ) index =                 0x0217;

	RECT rc;

	rc.left   = ( index % _CHARIMAGE_XNUM ) * _CHARIMAGE_W;
	rc.top    = ( index / _CHARIMAGE_XNUM ) * _CHARIMAGE_H;
	rc.right  = rc.left + _CHARIMAGE_W;
	rc.bottom = rc.top  + _CHARIMAGE_H;

	DxDraw_Put_Clip( p_rc_view, x, y, &rc, SfNo_FONT_1 );
}

#define _CHARACTOR_FONTWIDTH 6

int FontWork_PutNumber6( const RECT *rcView, int x, int y, int value, int flags )
{
	RECT rcNum[] ={
		{128,  0,136, 12},
		{136,  0,144, 12},
		{144,  0,152, 12},
		{152,  0,160, 12},
		{160,  0,168, 12},
		{168,  0,176, 12},
		{176,  0,184, 12},
		{184,  0,192, 12},
		{192,  0,200, 12},
		{200,  0,208, 12},
	};
	RECT rcMinus = {104,  0,112, 12};
	long tbl0[]  = {0, 1,10,100,1000,10000,100000};
	long tbl9[]  = {0, 9,99,999,9999,99999,999999};

	long a;
	BOOL bPut    = FALSE;
	BOOL bMinus  = FALSE;
	long offset  =     0;
	long xpos = x;
	long ypos = y;
	long keta = flags & 0x0f;
	long w     = 0;

	if( !keta ) return xpos;
	if( value > tbl9[ keta ] ) value = tbl9[ keta ];
	if( value < 0    ){ value *= -1; bMinus = TRUE; w += 1; }
	for( a = 6; a > 1; a-- ){ if( value >= tbl0[ a ] ) break; }	//幅を調べる
	w += a * _CHARACTOR_FONTWIDTH;

	// センタリング
//	if( flags & CHARACTORFLAG_CENTER ){ xpos -= w / 2; ypos -= 4; }

	for( offset = keta; offset > 0; offset-- )
	{
		a     = value / tbl0[ offset ];
		value = value % tbl0[ offset ];

		// '-'
		if( bMinus && a && !bPut )
		{
			DxDraw_Put_Clip( rcView, xpos - _CHARACTOR_FONTWIDTH, ypos, &rcMinus, SfNo_FONT_1 );
		}
		// num
		if( bPut || a || offset == 1 )
		{
			DxDraw_Put_Clip( rcView, xpos, ypos, &rcNum[ a ], SfNo_FONT_1 );
		}

		if(          a          ) bPut = TRUE;
		xpos += _CHARACTOR_FONTWIDTH;
	}

	return xpos;
}

int FontWork_PutText( const RECT* p_rc_view, int x, int y, const char* text, int flags )
{
	unsigned short       code;
	const unsigned char* p = (const unsigned char*)text;

	while( *p )
	{
		// 半角カナは使えない…
		if( *p & 0x80 )
		{
			code  = *p;
			code *= 0x100;
			p++;
			code += *p;
		}
		else
		{
			code = *p;
		}
		FontWork_Put( p_rc_view, x, y, code, flags );
		x += FontWork_GetWidth( code ) + 1;// padding
		p++;
	}

	return x;
}
