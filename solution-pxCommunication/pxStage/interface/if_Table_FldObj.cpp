#include <StdAfx.h>

#include <math.h>

#include "../../Dx/DxDraw.h"
#include "../../Fixture/KeyControl.h"
#include "../../DataTable/dt_Stage.h"

#include "SurfaceNo.h"
#include "Interface.h"
#include "if_Generic.h"
#include "if_Tools.h"
#include "cls_Scroll.h"

typedef struct
{
	BOOL bVisible;
	long x[ 2 ], y[ 2 ];

}_SELECTSTRUCT;

extern cls_SCROLL    g_ScrlMap_H;
extern cls_SCROLL    g_ScrlMap_V;
extern dt_Stage*     g_stage    ;

static unsigned long _bg_color;
static _SELECTSTRUCT _select;
static POINT         _cursor;

void if_Table_FldObj_Initialize( void )
{
	memset( &_select, 0, sizeof(_SELECTSTRUCT) );
	memset( &_cursor, 0, sizeof(POINT)         );
	_cursor.x = -1;
	_cursor.y = -1;
}

void if_Table_FldObj_SetCursor( int x, int y )
{
	x += g_ScrlMap_H.GetOffset();
	y += g_ScrlMap_V.GetOffset();
	_cursor.x =  x / FIELDGRID;
	_cursor.y =  y / FIELDGRID;
}

void if_Table_FldObj_GetCursor( int *p_x, int *p_y )
{
	*p_x = _cursor.x;
	*p_y = _cursor.y;
}


void if_Table_FldObj_SetSelect( int x1, int y1, int x2, int y2 )
{
	int off_x;
	int off_y;
	off_x = g_ScrlMap_H.GetOffset();
	off_y = g_ScrlMap_V.GetOffset();

	if( x1 >= 0 ) _select.x[ 0 ] = ( x1 + off_x ) / FIELDGRID;
	if( y1 >= 0 ) _select.y[ 0 ] = ( y1 + off_y ) / FIELDGRID;
	if( x2 >= 0 ) _select.x[ 1 ] = ( x2 + off_x ) / FIELDGRID;
	if( y2 >= 0 ) _select.y[ 1 ] = ( y2 + off_y ) / FIELDGRID;
}

void if_Table_FldObj_GetSelect( int *px1, int *py1, int *px2, int *py2 )
{
	if( px1 ) *px1 = _select.x[ 0 ];
	if( py1 ) *py1 = _select.y[ 0 ];
	if( px2 ) *px2 = _select.x[ 1 ];
	if( py2 ) *py2 = _select.y[ 1 ];
}

void if_Table_FldObj_VisibleSelect( BOOL b )
{
	_select.bVisible = b;
}


void if_Table_FldObj_Put_Table( RECT *rcView )
{
	int off_x, off_y, x, y;
	const DT_FLDOBJ* p_unit;

	dt_FldObj* p_units =g_stage->Unit_GetPointer();

	RECT rcUnit     = { 48,  0, 64, 16 };
	RECT rc;

	off_x = g_ScrlMap_H.GetOffset();
	off_y = g_ScrlMap_V.GetOffset();

	for( int u = 0; p_unit = p_units->Get( u ); u++ )
	{
		x = p_unit->x * FIELDGRID - off_x;
		y = p_unit->y * FIELDGRID - off_y;
		rc.left   = ( p_unit->param1 % UNITPARTS_NUM_H ) * FIELDGRID;
		rc.right  = rc.left + FIELDGRID;
		rc.top    = ( p_unit->param1 / UNITPARTS_NUM_H ) * FIELDGRID;
		rc.bottom = rc.top  + FIELDGRID;
		DxDraw_Put_Clip( &g_rcView, x, y, &rc    , SfNo_UNITTYPE );
		DxDraw_Put_Clip( &g_rcView, x, y, &rcUnit, SfNo_SYMBOL   );
	}
}

void if_Table_FldObj_Put_Cursor( RECT *rcView )
{
	int  x, y;
	int  off_x, off_y;
	int  x_start, x_end;
	int  y_start, y_end;
	RECT rc_cur_sel   = { 16, 32, 24, 40}; 
	RECT rc_cur_del   = { 32, 16, 48, 32};
	RECT rc_cur_touch = { 48, 16, 64, 32};
	int  rc_index;

	off_x = g_ScrlMap_H.GetOffset();
	off_y = g_ScrlMap_V.GetOffset();

	// ‘I‘ð”ÍˆÍ
	if( _select.bVisible )
	{
		RECT rcSelect[]   = { { 16, 16, 32, 32}, {32, 16, 48, 32} };

		if( _select.x[ 0 ] <= _select.x[ 1 ] ){ x_start = _select.x[ 0 ]; x_end = _select.x[ 1 ]; }
		else                                  { x_start = _select.x[ 1 ]; x_end = _select.x[ 0 ]; }
		if( _select.y[ 0 ] <= _select.y[ 1 ] ){ y_start = _select.y[ 0 ]; y_end = _select.y[ 1 ]; }
		else                                  { y_start = _select.y[ 1 ]; y_end = _select.y[ 0 ]; }
		switch( if_Tools_Mode_Get() ){
		case TOOLMODE_Pen  : rc_index = 0; break;
		case TOOLMODE_Erase: rc_index = 1; break;
		case TOOLMODE_Edit : rc_index = 0; break;
		}
		for( y = y_start; y <= y_end; y++ )
		{
			for( x = x_start; x <= x_end; x++ )
			{
				DxDraw_Put_Clip( &g_rcView, x * FIELDGRID  - off_x, y * FIELDGRID  - off_y, &rcSelect[ rc_index ], SfNo_SYMBOL );
			}
		}
		x = _cursor.x * FIELDGRID - off_x;
		y = _cursor.y * FIELDGRID - off_y;
		PutNumber6( rcView, x + 8, y    , _cursor.x, 3 );
		PutNumber6( rcView, x + 8, y + 8, _cursor.y, 3 );
	}
	else if( _cursor.x >= 0 )
	{
		dt_FldObj*             p_copy = g_stage->Unit_GetPointer_Copy();
		const DT_FLDOBJ* p_unit;
		RECT rcUnit       = { 32,  0, 48, 16};

		x = _cursor.x * FIELDGRID - off_x;
		y = _cursor.y * FIELDGRID - off_y;
		switch( if_Tools_Mode_Get() )
		{
		case TOOLMODE_Pen  :
			DxDraw_Put_Clip( rcView, x, y, &rc_cur_sel, SfNo_SYMBOL );
			for( int u  = 0; p_unit = p_copy->Get( u ); u++ )
			{
				DxDraw_Put_Clip( rcView, x + p_unit->x * FIELDGRID, y + p_unit->y * FIELDGRID, &rcUnit, SfNo_SYMBOL );
			}
			PutNumber6( rcView, x + 8, y    , _cursor.x, 3 );
			PutNumber6( rcView, x + 8, y + 8, _cursor.y, 3 );
			break;
		case TOOLMODE_Erase:
			DxDraw_Put_Clip( rcView, x, y, &rc_cur_del, SfNo_SYMBOL );
			break;
		case TOOLMODE_Edit :
			DxDraw_Put_Clip( rcView, x, y, &rc_cur_touch, SfNo_SYMBOL );
			break;
		}
	}
}

