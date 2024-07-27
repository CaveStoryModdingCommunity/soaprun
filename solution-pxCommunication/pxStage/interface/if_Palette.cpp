#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
//#include "../../Fixture/FilePath.h"
#include "../../DataTable/dt_Stage.h"

#include "Interface.h"
#include "SurfaceNo.h"
#include "cls_Scroll.h"
#include "if_Cursor.h"

#include "if_Table_Map.h"
#include "if_Table_FldObj.h"
//#include "if_Table_FieldDoor.h"
#include "if_Table_Attribute.h"
#include "if_Palette.h"

extern dt_Stage* g_stage;

#define _HANDLE_H 24
#define _FRAME_W  256
#define _FRAME_H  ( _HANDLE_H + MAPPARTS_BMP_H )

#define _BUTTON_CLOSE_X 144
#define _BUTTON_CLOSE_Y   4
#define _BUTTON_LOAD_X  124
#define _BUTTON_LOAD_Y    4
#define _BUTTON_SIZE     16

#define _BUTTON_PARTS_X 104
#define _BUTTON_PARTS_Y   4


typedef struct
{
	BOOL bVisible;
	long x[ 2 ], y[ 2 ];

}_SELECTSTRUCT;

static POINT         _frame;
static _SELECTSTRUCT _select;
static POINT         _cursor;
static RECT          _rcFrame;
static const char    *_frame_pos_name = "mapparts.pos";

static unsigned char _selected_parts_index = 0;  // attribute / door で使用

extern char g_dir_profile[];

void if_Palette_Initialize( HWND hWnd )
{
	FILE *fp;
	char path[MAX_PATH];

	sprintf( path, "%s\\%s", g_dir_profile, _frame_pos_name );
	fp = fopen( path, "rb" );
	if( fp )
	{
		fread( &_frame  , sizeof(POINT), 1, fp );
//		fread( &_frame.x, sizeof(long), 1, fp );
//		fread( &_frame.y, sizeof(long), 1, fp );
		fclose( fp );
	}
	else
	{
		RECT rc_client;

		GetClientRect( hWnd, &rc_client );
		_frame.x = rc_client.left + 8;
		_frame.y = rc_client.top   + 8;
	}

	memset( &_select, 0, sizeof(_SELECTSTRUCT) );
	memset( &_cursor, 0, sizeof(POINT        )  );
	_cursor.x = -1;

}


void if_Palette_SavePosition()
{
	FILE *fp;
	char path[MAX_PATH];

	sprintf( path, "%s\\%s", g_dir_profile, _frame_pos_name );
	fp = fopen( path, "wb" );
	if( fp )
	{
		fwrite( &_frame, sizeof(POINT), 1, fp  );
//		fwrite( &_frame.x, sizeof(long), 1, fp );
//		fwrite( &_frame.y, sizeof(long), 1, fp );
		fclose( fp );
	}
}

// 座標
void if_Palette_SetPosition( int x,   int y   )
{
	_frame.x = x;
	_frame.y = y;
	if( _frame.x < -_FRAME_W  + 8 ) _frame.x = -_FRAME_W  + 8;
	if( _frame.y < -_HANDLE_H + 8 ) _frame.y = -_HANDLE_H + 8;
	if( _frame.x > g_rcView.right  - SCROLLBUTTONSIZE - 8 )
		_frame.x = g_rcView.right  - SCROLLBUTTONSIZE - 8;
	if( _frame.y > g_rcView.bottom - SCROLLBUTTONSIZE - 8 )
		_frame.y = g_rcView.bottom - SCROLLBUTTONSIZE - 8;
}

void if_Palette_GetPosition( int *px, int *py )
{
	if( px ) *px = _frame.x;
	if( py ) *py = _frame.y;
}

void if_Palette_SetCursor( int x, int y )
{
	if( x - _frame.x < 0 || y - _frame.y - _HANDLE_H < 0 )
	{
		_cursor.x = -1;
		_cursor.y = -1;
	}
	else
	{
		_cursor.x =  ( x - _frame.x             ) / FIELDGRID;
		_cursor.y =  ( y - _frame.y - _HANDLE_H ) / FIELDGRID;
	}
}

void if_Palette_SetSelect( int x1, int y1, int x2, int y2 )
{
	if( x1 >= 0 ) _select.x[ 0 ] =  ( x1 - _frame.x             ) / FIELDGRID;
	if( y1 >= 0 ) _select.y[ 0 ] =  ( y1 - _frame.y - _HANDLE_H ) / FIELDGRID;
	if( x2 >= 0 ) _select.x[ 1 ] =  ( x2 - _frame.x             ) / FIELDGRID;
	if( y2 >= 0 ) _select.y[ 1 ] =  ( y2 - _frame.y - _HANDLE_H ) / FIELDGRID;
}

void if_Palette_GetSelect( int *px1, int *py1, int *px2, int *py2 )
{
	if( px1 ) *px1 = _select.x[ 0 ];
	if( py1 ) *py1 = _select.y[ 0 ];
	if( px2 ) *px2 = _select.x[ 1 ];
	if( py2 ) *py2 = _select.y[ 1 ];
}

void if_Palette_VisibleSelect( BOOL b )
{
	_select.bVisible = b;
}

// 領域
void if_Palette_SetRect( void )
{
	if( _frame.x < -_FRAME_W  + 8 ) _frame.x = -_FRAME_W  + 8;
	if( _frame.y < -_HANDLE_H + 8 ) _frame.y = -_HANDLE_H + 8;
	if( _frame.x > g_rcView.right  - SCROLLBUTTONSIZE - 8 )
		_frame.x = g_rcView.right  - SCROLLBUTTONSIZE - 8;
	if( _frame.y > g_rcView.bottom - SCROLLBUTTONSIZE - 8 )
		_frame.y = g_rcView.bottom - SCROLLBUTTONSIZE - 8;

	_rcFrame.left   = _frame.x;
	_rcFrame.top    = _frame.y;
	_rcFrame.right  = _frame.x + _FRAME_W;
	_rcFrame.bottom = _frame.y + _FRAME_H;
}

RECT *if_Palette_GetRect( void )
{
	return &_rcFrame;
}


bool if_Palette_IsCloseButton( int x, int y )
{
	if( x >= _frame.x + _BUTTON_CLOSE_X && x <  _frame.x + _BUTTON_CLOSE_X + _BUTTON_SIZE &&
		y >= _frame.y + _BUTTON_CLOSE_Y && y <  _frame.y + _BUTTON_CLOSE_Y + _BUTTON_SIZE ) return true;
	return false;
}

bool if_Palette_IsLoadButton(  int x, int y )
{
	if( x >= _frame.x + _BUTTON_LOAD_X && x <  _frame.x + _BUTTON_LOAD_X + _BUTTON_SIZE &&
		y >= _frame.y + _BUTTON_LOAD_Y && y <  _frame.y + _BUTTON_LOAD_Y + _BUTTON_SIZE ) return true;
	return false;
}

bool if_Palette_IsMapParts(  int x, int y )
{  
	if( x >= _frame.x                  &&
		x <  _frame.x + MAPPARTS_BMP_W &&
		y >= _frame.y + _HANDLE_H      &&
		y <  _frame.y + _FRAME_H         )
		return true;
	return false;
}

void PutNumber6( RECT *rcView, int x, int y, int value, unsigned int flag );

void if_Palette_Put( enum_EditMode mode )
{
	int i, x = 0, y = 0, w, l;
	int index;
	int surf;
	RECT rcHandle  = { 0, 0,256,24};
	RECT rcPalette = { 0, 0,  0, 0};
	RECT rcBack    = { 0,24,256,40};
	RECT rcCursor  = { 0, 0, 16,16};
	RECT rcSelect  = {16, 0, 32,16};

	int parts_height;


	DxDraw_Put_Clip( &g_rcView, _frame.x, _frame.y, &rcHandle, SfNo_PALETTE );
	switch( mode )
	{
	case enum_EditMode_Map  :
		g_stage->Map_GetSize( &w, &l );
		index        = g_stage->Map_copy_GetPartsIndex( 0 );
		surf         = SfNo_MAPPARTS;
		parts_height = FIELDGRID;
		if_Table_Map_GetCursor( &x, &y );
		break;

	case enum_EditMode_FldObj:
		g_stage->Map_GetSize( &w, &l );
		index        = g_stage->Unit_copy_GetUnitNo();
		surf         = SfNo_UNITTYPE ;
		parts_height = FIELDGRID;
		if_Table_FldObj_GetCursor( &x, &y );
		break;

/*	case enum_EditMode_FieldDoor:
		g_stage->Map_GetSize( &w, &l );
		index = _selected_parts_index;
		surf  = SfNo_DOOR;
		if_Table_FieldDoor_GetCursor( &x, &y );
		parts_height = FIELDGRID * 2;
		break;
*/
	case enum_EditMode_Attribute:
		g_stage->Attribute_GetSize( &w, &l );
		index            = _selected_parts_index;
		if( EditMode_Layer_Get() != 0 ) surf = SfNo_ATTRIBUTE2;
		else                            surf = SfNo_ATTRIBUTE1;
		parts_height     = FIELDGRID;
		rcPalette.bottom = 16;
		if_Table_Attribute_GetCursor( &x, &y );
		break;

	case enum_EditMode_Resident:
		index = w = l = x = y = 0;
		surf  = SfNo_UNIT_lCast;
		parts_height     = 0;
		break;
	}

	// 数値の表示
	PutNumber6( &g_rcView, _frame.x +  8, _frame.y +  4, x, 5 );
	PutNumber6( &g_rcView, _frame.x +  8, _frame.y + 12, y, 5 );
	PutNumber6( &g_rcView, _frame.x + 56, _frame.y +  4, w, 5 );
	PutNumber6( &g_rcView, _frame.x + 56, _frame.y + 12, l, 5 );

	// 網背景
	for( i = 0; i < MAPPARTS_NUM_V; i++ )
		DxDraw_Put_Clip( &g_rcView, _frame.x, _frame.y + _HANDLE_H + i * FIELDGRID, &rcBack, SfNo_PALETTE );

	// 現在選択中のパーツ
	RECT rc;

	rc.left   = ( index % MAPPARTS_NUM_H ) * FIELDGRID;
	rc.top    = ( index / MAPPARTS_NUM_H ) * parts_height;
	rc.right  = rc.left + FIELDGRID;
	rc.bottom = rc.top  + FIELDGRID;
	DxDraw_SubSurface_GetImageSize( &rcPalette.right, &rcPalette.bottom, surf );
	DxDraw_Put_Clip( &g_rcView, _frame.x + _BUTTON_PARTS_X, _frame.y + _BUTTON_PARTS_Y, &rc , surf );
	DxDraw_Put_Clip( &g_rcView, _frame.x                  , _frame.y + _HANDLE_H, &rcPalette, surf );

	// 選択範囲
	int x_start, x_end;
	int y_start, y_end;

	if( _select.bVisible )
	{
		if( _select.x[ 0 ] <= _select.x[ 1 ] ){ x_start = _select.x[ 0 ]; x_end = _select.x[ 1 ]; }
		else                                  { x_start = _select.x[ 1 ]; x_end = _select.x[ 0 ]; }
		if( _select.y[ 0 ] <= _select.y[ 1 ] ){ y_start = _select.y[ 0 ]; y_end = _select.y[ 1 ]; }
		else                                  { y_start = _select.y[ 1 ]; y_end = _select.y[ 0 ]; }
		if( x_start < 0 ) x_start = 0;
		if( y_start < 0 ) y_start = 0;
		if( x_end   >= MAPPARTS_NUM_H ) x_end = MAPPARTS_NUM_H - 1;
		if( y_end   >= MAPPARTS_NUM_V ) y_end = MAPPARTS_NUM_V - 1;
		for( y = y_start; y <= y_end; y++ )
		{
			for( x = x_start; x <= x_end; x++ )
			{
				DxDraw_Put_Clip( &g_rcView,
					x * FIELDGRID + _frame.x,
					y * FIELDGRID + _frame.y + _HANDLE_H,
					&rcSelect, SfNo_SYMBOL );
			}
		}
	}

	if( _cursor.x >= 0 && _cursor.y >= 0 )
	{
		DxDraw_Put_Clip( &g_rcView,
			_cursor.x * FIELDGRID + _frame.x,
			_cursor.y * FIELDGRID + _frame.y + _HANDLE_H,
			&rcCursor, SfNo_SYMBOL );
	}
}


void if_Palette_SetSelectedPartsIndex( unsigned char parts_index )
{
	_selected_parts_index = parts_index;
}
unsigned char if_Palette_GetSelectedPartsIndex( void )
{
	return _selected_parts_index;
}



