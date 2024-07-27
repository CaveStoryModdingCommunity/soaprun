#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
//#include "../../Fixture/FilePath.h"
#include "../../DataTable/dt_Stage.h"

#include "Interface.h"
#include "SurfaceNo.h"
#include "cls_Scroll.h"
#include "if_Cursor.h"

#include "if_Table_Map.h"

#include "if_Tools.h"

#define _FRAME_W  192
#define _FRAME_H  40

#define _BUTTON_PEN_X       12
#define _BUTTON_PEN_Y        4
#define _BUTTON_ERASE_X     32
#define _BUTTON_ERASE_Y      4
#define _BUTTON_EDIT_X      52
#define _BUTTON_EDIT_Y       4

#define _BUTTON_VISIBLE_X   88
#define _BUTTON_VISIBLE_Y    4

//#define _BUTTON_VISI_DOOR_X  152
//#define _BUTTON_VISI_DOOR_Y    4
#define _BUTTON_VISI_ATTR_X  168
#define _BUTTON_VISI_ATTR_Y    4

#define _BUTTON_EDITLAYER_X 88
#define _BUTTON_EDITLAYER_Y 20

#define _BUTTON_SIZE     16

#define _BUTTON_PARTS_X 104
#define _BUTTON_PARTS_Y   4



static POINT  _frame;
static RECT   _rcFrame;
static const char    *_frame_pos_name = "toolsframe.pos";

extern char g_dir_profile[];

static TOOLMODE _tool_mode = TOOLMODE_Pen;
static bool     _visibles[ MAPLAYERNUM ] = { true };//, true, true };
//static bool     _b_door_visible          =   true ;
static bool     _b_attr_visible          =   false;
static int      _edit_layer = 0;

TOOLMODE if_Tools_Mode_Get( void          ){ return _tool_mode; }
void     if_Tools_Mode_Set( TOOLMODE mode ){ _tool_mode = mode; }

void if_Tools_Visible_Reverse( int layer )
{
	if( layer < 0 || layer >= MAPLAYERNUM ) return;
	_visibles[ layer ] = _visibles[ layer ] ? false : true;

	if     ( _visibles[ layer ]   )
	{
		if_Tools_EditLayer_Set( layer );
	}
	else if( layer == _edit_layer )
	{
		for( int l = MAPLAYERNUM - 1; l >= 0; l-- )
		{
			if( _visibles[ l ] )
			{
				if_Tools_EditLayer_Set( l );
				break;
			}
		}
	}
}

bool if_Tools_Visible_Get( int layer )
{
	if( layer < 0 || layer >= MAPLAYERNUM ) return false;
	return _visibles[ layer ];
}

//void if_Tools_DoorVisible_Reverse(){ _b_door_visible = _b_door_visible ? false : true; }
//bool if_Tools_DoorVisible_Is     (){ return _b_door_visible; }
void if_Tools_AttrVisible_Reverse(){ _b_attr_visible = _b_attr_visible ? false : true; }
bool if_Tools_AttrVisible_Is     (){ return _b_attr_visible; }

int  if_Tools_EditLayer_Get( void      ){ return  _edit_layer; }
void if_Tools_EditLayer_Set( int layer )
{
	_edit_layer = layer;

	if( layer == MAPLAYERNUM )
	{
		for( int l = 0; l < MAPLAYERNUM; l++ ) _visibles[ l ] = true;
	}
	else
	{
		_visibles[ layer ] = true;
	}
}

void if_Tools_Initialize( HWND hWnd )
{
	FILE *fp;
	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s", g_dir_profile, _frame_pos_name );
	fp = fopen( path, "rb" );
	if( fp )
	{
		fread( &_frame, sizeof(POINT), 1, fp );
		fclose( fp );
	}
	else
	{
		RECT rc_client;

		GetClientRect( hWnd, &rc_client );
		_frame.x = rc_client.left + 8;
		_frame.y = rc_client.top  + 8;
	}


}


void if_Tools_SavePosition()
{
	FILE *fp;
	char path[MAX_PATH];

	sprintf( path, "%s\\%s", g_dir_profile, _frame_pos_name );
	fp = fopen( path, "wb" );
	if( fp )
	{
		fwrite( &_frame, sizeof(POINT), 1, fp );
		fclose( fp );
	}
}

// À•W
void if_Tools_SetPosition( int x,   int y   )
{
	_frame.x = x;
	_frame.y = y;
	if( _frame.x < -_FRAME_W + 8 ) _frame.x = -_FRAME_W + 8;
	if( _frame.y < -_FRAME_H + 8 ) _frame.y = -_FRAME_H + 8;
	if( _frame.x > g_rcView.right  - SCROLLBUTTONSIZE - 8 )
		_frame.x = g_rcView.right  - SCROLLBUTTONSIZE - 8;
	if( _frame.y > g_rcView.bottom - SCROLLBUTTONSIZE - 8 )
		_frame.y = g_rcView.bottom - SCROLLBUTTONSIZE - 8;
}

void if_Tools_GetPosition( int *px, int *py )
{
	if( px ) *px = _frame.x;
	if( py ) *py = _frame.y;
}


// —Ìˆæ
void if_Tools_SetRect( void )
{
	if( _frame.x <  8 ) _frame.x =  8;
	if( _frame.y <  8 ) _frame.y =  8;
	if( _frame.x > g_rcView.right  - SCROLLBUTTONSIZE - 8 )
		_frame.x = g_rcView.right  - SCROLLBUTTONSIZE - 8;
	if( _frame.y > g_rcView.bottom - SCROLLBUTTONSIZE - 8 )
		_frame.y = g_rcView.bottom - SCROLLBUTTONSIZE - 8;

	_rcFrame.left   = _frame.x;
	_rcFrame.top    = _frame.y;
	_rcFrame.right  = _frame.x + _FRAME_W;
	_rcFrame.bottom = _frame.y + _FRAME_H;
}

RECT *if_Tools_GetRect( void )
{
	return &_rcFrame;
}

TOOLMODE if_Tools_GetHitButton_Mode( int x, int y )
{
	if(      x >= _frame.x + _BUTTON_PEN_X   && x <  _frame.x + _BUTTON_PEN_X   + _BUTTON_SIZE &&
		     y >= _frame.y + _BUTTON_PEN_Y   && y <  _frame.y + _BUTTON_PEN_Y   + _BUTTON_SIZE ) return TOOLMODE_Pen;
	else if( x >= _frame.x + _BUTTON_ERASE_X && x <  _frame.x + _BUTTON_ERASE_X + _BUTTON_SIZE &&
		     y >= _frame.y + _BUTTON_ERASE_Y && y <  _frame.y + _BUTTON_ERASE_Y + _BUTTON_SIZE ) return TOOLMODE_Erase;
	else if( x >= _frame.x + _BUTTON_EDIT_X  && x <  _frame.x + _BUTTON_EDIT_X  + _BUTTON_SIZE &&
		     y >= _frame.y + _BUTTON_EDIT_Y  && y <  _frame.y + _BUTTON_EDIT_Y  + _BUTTON_SIZE ) return TOOLMODE_Edit;

	return TOOLMODE_None;
}

int if_Tools_GetHitButton_Visible( int x, int y )
{
	if(      x >= _frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * 0  &&
			 x <  _frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * 1  &&
		     y >= _frame.y + _BUTTON_VISIBLE_Y                     &&
			 y <  _frame.y + _BUTTON_VISIBLE_Y + _BUTTON_SIZE ) return 0;
	else if( x >= _frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * 1  &&
			 x <  _frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * 2  &&
		     y >= _frame.y + _BUTTON_VISIBLE_Y                     &&
			 y <  _frame.y + _BUTTON_VISIBLE_Y + _BUTTON_SIZE ) return 1;
	else if( x >= _frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * 2  &&
			 x <  _frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * 3  &&
		     y >= _frame.y + _BUTTON_VISIBLE_Y                     &&
			 y <  _frame.y + _BUTTON_VISIBLE_Y + _BUTTON_SIZE ) return 2;
	return -1;
}
/*
bool if_Tools_DoorVisible_GetHitButton( int x, int y )
{
	if(      x >= _frame.x + _BUTTON_VISI_DOOR_X + _BUTTON_SIZE * 0  &&
			 x <  _frame.x + _BUTTON_VISI_DOOR_X + _BUTTON_SIZE * 1  &&
		     y >= _frame.y + _BUTTON_VISI_DOOR_Y                     &&
			 y <  _frame.y + _BUTTON_VISI_DOOR_Y + _BUTTON_SIZE ) return true;
	return false;
}
*/
bool if_Tools_AttrVisible_GetHitButton( int x, int y )
{
	if(      x >= _frame.x + _BUTTON_VISI_ATTR_X + _BUTTON_SIZE * 0  &&
			 x <  _frame.x + _BUTTON_VISI_ATTR_X + _BUTTON_SIZE * 1  &&
		     y >= _frame.y + _BUTTON_VISI_ATTR_Y                     &&
			 y <  _frame.y + _BUTTON_VISI_ATTR_Y + _BUTTON_SIZE ) return true;
	return false;
}

int if_Tools_GetHitButton_EditLayer( int x, int y )
{
	if(      x >= _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 0  &&
			 x <  _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 1  &&
		     y >= _frame.y + _BUTTON_EDITLAYER_Y                     &&
			 y <  _frame.y + _BUTTON_EDITLAYER_Y + _BUTTON_SIZE ) return 0;
	else if( x >= _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 1  &&
			 x <  _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 2  &&
		     y >= _frame.y + _BUTTON_EDITLAYER_Y                     &&
			 y <  _frame.y + _BUTTON_EDITLAYER_Y + _BUTTON_SIZE ) return 1;
	else if( x >= _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 2  &&
			 x <  _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 3  &&
		     y >= _frame.y + _BUTTON_EDITLAYER_Y                     &&
			 y <  _frame.y + _BUTTON_EDITLAYER_Y + _BUTTON_SIZE ) return 2;
	else if( x >= _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 3  &&
			 x <  _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * 4  &&
		     y >= _frame.y + _BUTTON_EDITLAYER_Y                     &&
			 y <  _frame.y + _BUTTON_EDITLAYER_Y + _BUTTON_SIZE ) return 3;
	return -1;
}

bool if_Tools_IsTools(  int x, int y )
{
	if( x >= _frame.x && x <  _frame.x &&
		y >= _frame.y && y <  _frame.y ) return true;
	return false;
}


void if_Tools_Put( void )
{
	RECT rcFrame  = { 0, 0, 192, 40};
	RECT rcSelect = { 0, 40, 16, 56};
	RECT rcInvisible[] = {
		{ 16, 40, 32, 56},
		{ 32, 40, 48, 56},
		{ 48, 40, 64, 56},
	};

//	RECT rc_door_visible = { 80, 40, 96, 56};
	RECT rc_attr_visible = { 96, 40,112, 56};
	int x, y;

	RECT rcEdit = { 64, 40, 80, 56};

	DxDraw_Put_Clip( &g_rcView, _frame.x, _frame.y, &rcFrame, SfNo_TOOLS );
	switch( _tool_mode )
	{
	case TOOLMODE_Pen  : x = _frame.x + _BUTTON_PEN_X  ; y = _frame.y + _BUTTON_PEN_Y  ; break;
	case TOOLMODE_Erase: x = _frame.x + _BUTTON_ERASE_X; y = _frame.y + _BUTTON_ERASE_Y; break;
	case TOOLMODE_Edit : x = _frame.x + _BUTTON_EDIT_X ; y = _frame.y + _BUTTON_EDIT_Y ; break;
	}
	DxDraw_Put_Clip( &g_rcView, x, y, &rcSelect, SfNo_TOOLS );

	y = _frame.y + _BUTTON_EDITLAYER_Y;
	x = _frame.x + _BUTTON_EDITLAYER_X + _BUTTON_SIZE * _edit_layer;

	DxDraw_Put_Clip( &g_rcView, x, y, &rcEdit, SfNo_TOOLS );

	for( int i = 0; i < MAPLAYERNUM; i++ )
	{
		if( _visibles[ i ] )
		{
			DxDraw_Put_Clip( &g_rcView,
				_frame.x + _BUTTON_VISIBLE_X + _BUTTON_SIZE * i,
				_frame.y + _BUTTON_VISIBLE_Y, &rcInvisible[ i ], SfNo_TOOLS );
		}
	}

//	if( _b_door_visible ) DxDraw_Put_Clip( &g_rcView, _frame.x + _BUTTON_VISI_DOOR_X, _frame.y + _BUTTON_VISI_DOOR_Y, &rc_door_visible, SfNo_TOOLS );
	if( _b_attr_visible ) DxDraw_Put_Clip( &g_rcView, _frame.x + _BUTTON_VISI_ATTR_X, _frame.y + _BUTTON_VISI_ATTR_Y, &rc_attr_visible, SfNo_TOOLS );
}





