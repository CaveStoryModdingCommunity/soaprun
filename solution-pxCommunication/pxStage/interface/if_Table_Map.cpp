#include <StdAfx.h>

//#include <math.h>

#include "../../Dx/DxDraw.h"
#include "../../Fixture/KeyControl.h"
#include "../../DataTable/dt_Stage.h"

#include "../EditMode.h"

#include "SurfaceNo.h"
#include "Interface.h"
#include "if_Generic.h"
#include "if_Tools.h"
#include "cls_Scroll.h"

extern dt_Stage* g_stage;

typedef struct
{
	BOOL bVisible;
	long x[ 2 ], y[ 2 ];

}_SELECTSTRUCT;

static RECT           _rcField;
extern cls_SCROLL     g_ScrlMap_H;
extern cls_SCROLL     g_ScrlMap_V;
static unsigned long  _bg_color;
static _SELECTSTRUCT  _select;
static POINT          _cursor;


void if_Table_Map_Initialize( void )
{
	int w, l;

	g_stage->Map_GetSize( &w, &l );

	g_ScrlMap_H.SetSize( w * FIELDGRID ); g_ScrlMap_H.SetMove( FIELDGRID, FIELDGRID * 4  );
	g_ScrlMap_V.SetSize( l * FIELDGRID ); g_ScrlMap_V.SetMove( FIELDGRID, FIELDGRID * 4  );
	_bg_color      = DxDraw_GetPaintColor( RGB(0x60, 0x60, 0x60) );

	memset( &_select, 0, sizeof(_SELECTSTRUCT) );
	memset( &_cursor, 0, sizeof(POINT        ) );
	_cursor.x = -1;

}


void if_Table_Map_SetRect( RECT *rcView )
{
	_rcField.left   = rcView->left;
	_rcField.right  = rcView->right  - SCROLLBUTTONSIZE;
	_rcField.top    = rcView->top;
	_rcField.bottom = rcView->bottom - SCROLLBUTTONSIZE;

	g_ScrlMap_H.SetRect( &_rcField );
	g_ScrlMap_V.SetRect( &_rcField );
}

RECT *if_Table_Map_GetRect( void )
{
	return &_rcField;
}

void if_Table_Map_SetCursor( int x, int y )
{
	int w, l;

	g_stage->Map_GetSize( &w, &l );

	x += g_ScrlMap_H.GetOffset();
	y += g_ScrlMap_V.GetOffset();

	_cursor.x =  ( x ) / FIELDGRID;
	_cursor.y =  ( y ) / FIELDGRID;

	if( _cursor.x < 0 ) _cursor.x = 0;
	if( _cursor.y < 0 ) _cursor.y = 0;
	if( _cursor.x > w -1 )
		_cursor.x = w -1;
	if( _cursor.y > l -1 )
		_cursor.y = l -1;
}

void if_Table_Map_GetCursor( int *p_x, int *p_y )
{
	*p_x = _cursor.x;
	*p_y = _cursor.y;
}

void if_Table_Map_SetSelect( int x1, int y1, int x2, int y2 )
{
	long off_x;
	long off_y;
	off_x = g_ScrlMap_H.GetOffset();
	off_y = g_ScrlMap_V.GetOffset();

	if( x1 >= 0 ) _select.x[ 0 ] =  ( x1 + off_x ) / FIELDGRID;
	if( y1 >= 0 ) _select.y[ 0 ] =  ( y1 + off_y ) / FIELDGRID;
	if( x2 >= 0 ) _select.x[ 1 ] =  ( x2 + off_x ) / FIELDGRID;
	if( y2 >= 0 ) _select.y[ 1 ] =  ( y2 + off_y ) / FIELDGRID;
}

void if_Table_Map_GetSelect( int *px1, int *py1, int *px2, int *py2 )
{
	if( px1 ) *px1 = _select.x[ 0 ];
	if( py1 ) *py1 = _select.y[ 0 ];
	if( px2 ) *px2 = _select.x[ 1 ];
	if( py2 ) *py2 = _select.y[ 1 ];
}

void if_Table_Map_VisibleSelect( BOOL b )
{
	_select.bVisible = b;
}


void if_Table_Map_Put_Table( RECT *rcView )
{
	int  off_x, off_y;
	int w, l;

	DxDraw_PaintRect( &_rcField, _bg_color );

	g_stage->Map_GetSize( &w, &l );

	g_ScrlMap_H.SetSize( w * FIELDGRID );
	g_ScrlMap_V.SetSize( l * FIELDGRID );
	off_x = g_ScrlMap_H.GetOffset();
	off_y = g_ScrlMap_V.GetOffset();

	if( if_Tools_Visible_Get( 0 ) ) g_stage->Map_Put ( 0, &_rcField, off_x, off_y, true  );
//	if( if_Tools_Visible_Get( 1 ) ) g_stage->Map_Put ( 1, &_rcField, off_x, off_y, false );
//	if( if_Tools_DoorVisible_Is() ) g_stage->Door_Put(    &_rcField, off_x, off_y, false );
//	if( if_Tools_DoorVisible_Is() ) g_stage->Door_Put(    &_rcField, off_x, off_y, true  );
//	if( if_Tools_Visible_Get( 2 ) ) g_stage->Map_Put ( 2, &_rcField, off_x, off_y, false );

	if( if_Tools_AttrVisible_Is() )
	{
		if( if_Tools_Visible_Get( 0 ) ) g_stage->Map_Put_Attribute( 0, &_rcField, off_x, off_y );
//		if( if_Tools_Visible_Get( 1 ) ) g_stage->Map_Put_Attribute( 1, &_rcField, off_x, off_y );
//		if( if_Tools_Visible_Get( 2 ) ) g_stage->Map_Put_Attribute( 2, &_rcField, off_x, off_y );
	}
}

void if_Table_Map_Put_Cursor( RECT *rcView )
{
	int x, y;
	int off_x, off_y;
	int x_start, x_end;
	int y_start, y_end;
	RECT rcSelect = {16,16,32,32};
	RECT rcCursor = { 0,16,16,32};

	off_x = g_ScrlMap_H.GetOffset();
	off_y = g_ScrlMap_V.GetOffset();

	// ‘I‘ð”ÍˆÍ
	if( _select.bVisible )
	{
		if( _select.x[ 0 ] <= _select.x[ 1 ] ){ x_start = _select.x[ 0 ]; x_end = _select.x[ 1 ]; }
		else                                  { x_start = _select.x[ 1 ]; x_end = _select.x[ 0 ]; }
		if( _select.y[ 0 ] <= _select.y[ 1 ] ){ y_start = _select.y[ 0 ]; y_end = _select.y[ 1 ]; }
		else                                  { y_start = _select.y[ 1 ]; y_end = _select.y[ 0 ]; }
		for( y = y_start; y <= y_end; y++ )
		{
			for( x = x_start; x <= x_end; x++ )
			{
				DxDraw_Put_Clip( &g_rcView,
					x * FIELDGRID  - off_x,
					y * FIELDGRID  - off_y,
					&rcSelect, SfNo_SYMBOL );
			}
		}
	}
	else if( _cursor.x >= 0 )
	{
		g_stage->Map_copy_GetSize( &x_end, &y_end, if_Tools_EditLayer_Get() );

		x_start = _cursor.x * FIELDGRID;
		y_start = _cursor.y * FIELDGRID;
		for( y = 0; y < y_end; y++ )
		{
			for( x = 0; x < x_end; x++ )
			{
				DxDraw_Put_Clip( &g_rcView,
					x_start + x * FIELDGRID  - off_x,
					y_start + y * FIELDGRID  - off_y,
					&rcCursor, SfNo_SYMBOL );
			}
		}
	}
}

void if_Table_Map_Put_Scroll( RECT *rcView )
{
	RECT rcDummy = {0,0,16,16};
	g_ScrlMap_H.Put( &_rcField );
	g_ScrlMap_V.Put( &_rcField );
	DxDraw_Put( _rcField.right, _rcField.bottom, &rcDummy, SfNo_DUMMY );
}

void if_Table_Map_Put_CursorPosition( int cur_x, int cur_y )
{
	long pos_x, pos_y;
	pos_x = g_ScrlMap_H.GetOffset() + cur_x;
	pos_y = g_ScrlMap_V.GetOffset() + cur_y;
	PutNumber6( &_rcField, 8,  8, pos_x, 6 );
	PutNumber6( &_rcField, 8, 16, pos_y, 6 );
}

