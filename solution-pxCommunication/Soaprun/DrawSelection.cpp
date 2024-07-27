#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#include "utility_Draw/Pane.h"
#include "utility_Sound/SEFF.h"

#include "DrawSelection.h"


DrawSelection::DrawSelection()
{
	int x, y;

	_pane_frame  = new Pane( DRAWCOLOR_NUM + 1 );

	for( int c = 0; c < DRAWCOLOR_NUM; c++ )
	{
		x = _CHIPS_X + 24 * c;
		y = _CHIPS_Y         ;
		_pane_chips[ c ] = new Pane();
		_pane_chips[ c ]->SetType_Image( x, y, 16, 16, SfNo_MapParts );
		_pane_chips[ c ]->SetAnimation ( c + 12, 0, 1, 0 );

		_pane_frame->Child_Add( _pane_chips[ c ] );
	}
	_pane_cursor = new Pane();
	_pane_cursor->SetType_Image( 0, 0, 8, 12, SfNo_FONT_1 );
	_pane_cursor->SetAnimation( 30, 8, 2, 16 );

	_pane_frame->Child_Add( _pane_cursor );
	
	_sel = 0;
}

DrawSelection::~DrawSelection()
{
	if( _pane_frame  ) delete _pane_frame ;
	if( _pane_cursor ) delete _pane_cursor;
	for( int c = 0; c < UNITCOLOR_NUM; c++ )
	{
		if( _pane_chips[ c ]  ) delete _pane_chips[ c ];
	}
}

void DrawSelection::_UpdateCursor()
{
	_pane_cursor->SetPos( _CHIPS_X - 8 + 24 * _sel, _CHIPS_Y + 2 );
}

void DrawSelection::Open()
{
//	_sel = 0;
	_UpdateCursor();
	_pane_frame->SetType_Frame( 8 + 24 * UNITCOLOR_NUM + 16, 16 + 16, DxDraw_GetSurfaceColor( SfNo_FRAME_B ), SfNo_FRAME_H, SfNo_FRAME_V );
	_pane_frame->SetPos( 8, FIELDVIEW_H - 80 );
}
void DrawSelection::Close()
{
	_pane_frame->Visible( false );
}

Pane* DrawSelection::GetPane()
{
	return _pane_frame;
}

void DrawSelection::Action( const BUTTONSSTRUCT* p_btns )
{
	bool b_click = false;

	if( p_btns->trg & KEY_BUTTON_RIGHT ){ _sel++; b_click = true; }
	if( p_btns->trg & KEY_BUTTON_LEFT  ){ _sel--; b_click = true; }

	if( b_click ) SEFF_Voice_Play( SeNo_Select );

	if( _sel < 0 ) _sel = UNITCOLOR_NUM - 1;
	if( _sel >= UNITCOLOR_NUM ) _sel = 0; 
	_UpdateCursor();
}

int DrawSelection::GetSelect()
{
	return _sel + 12;
}
