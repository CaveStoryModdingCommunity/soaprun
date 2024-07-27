#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
#include "../../Fixture/ShiftJIS.h"

#include "../utility_Sound/SEFF.h"

#include "MessageFrame.h"
#include "FontWork.h"

#define DIALOGUEFLAGS_VISIBLE  0x01
#define DIALOGUEFLAGS_FRAME    0x02
//#define DIALOGUEFLAGS_GRAY     0x04



#define DIALOGUE_NEXTLINEOFFSET (FONTWORK_FONTH + DIALOGUE_PADDING_Y)



// コンストラクタ
MessageCell::MessageCell( void ){ x = 0; code = 0; }

// 代入演算子
MessageCell& MessageCell::operator=( const MessageCell& other )
{
	if( this != &other )
	{
		x     = other.x    ;
		code  = other.code ;
	}
	return *this;
}

void MessageCell::Reset()
{
	x     = 0;
	code  = 0;
}

long MessageCell::Set( unsigned short code, long x )
{
	MessageCell::code = code;
	MessageCell::x    = x   ;

	return FontWork_GetWidth( code );
}

void MessageCell::Put( const RECT* p_rc, long y )
{
	if( code ) FontWork_Put( p_rc, p_rc->left + x, y, code, 0 );
}


// MessageLine ##################################

// コンストラクタ
MessageLine::MessageLine()
{
	_cells      = NULL;
	_cell_num   =    0;
	_scan_x     =    0;
	_scan_index =    0;
}

// デストラクタ
MessageLine::~MessageLine()
{
	if( _cells ) delete[] _cells;
	_cell_num = 0;
}

bool MessageLine::CopyCellsBy( const MessageLine& other )
{
	if( this == &other               ) return false;
	if( other._cell_num != _cell_num ) return false;
	for( long c = 0; c < _cell_num; c++ )
	{
		_cells[ c ] = other._cells[ c ];
	}
	return true;
}

void MessageLine::Initialize( long num, long i )
{
	_cells    = new MessageCell[ num ];
	_cell_num = num;
	_index    = i  ;
}

void MessageLine::Clear()
{
	for( long c = 0; c < _cell_num; c++ ) _cells[ c ].Reset();
	_scan_x     = 0;
	_scan_index = 0;
}

long MessageLine::SetCell( unsigned short code )
{
	if( _scan_index >= _cell_num ) return 0;

	_scan_x += _cells[ _scan_index ].Set( code, _scan_x ) + DIALOGUE_PADDING_X;
	_scan_index++;
	return _scan_x;
}

void MessageLine::Put( const RECT* p_rc, long scroll_y )
{
	for( long c = 0; c < _cell_num; c++ )
	{
		_cells[ c ].Put( p_rc, p_rc->top + scroll_y + _index * DIALOGUE_NEXTLINEOFFSET );
	}
}

// MessageFrame ##################################

bool MessageFrame::SetPosition( int num_h, int num_v, POSFROM set, int x, int y )
{
	if( num_v > line_max ) return false;
	if( num_v <        1 ) return false;
	line_num = num_v;

	// 必要はば
	long w, h;
	w = ( FONTWORK_FONTW_W + DIALOGUE_PADDING_X ) * num_h - DIALOGUE_PADDING_X + ( gframe ? (GENERICFRAME_W * 2) : 0 );
	h = ( FONTWORK_FONTH   + DIALOGUE_PADDING_Y ) * num_v - DIALOGUE_PADDING_Y + ( gframe ? (GENERICFRAME_H * 2) : 0 );

	switch( set )
	{
	case POSFROM_LT  :                             break;
	case POSFROM_RT  : x = WINDOW_W - w - x;     break;
	case POSFROM_LB  : y = WINDOW_H - h - y;     break;
	case POSFROM_RB  : x = WINDOW_W - w - x;
					   y = WINDOW_H - h - y;     break;
	case POSFROM_HC_T: x = ( WINDOW_W - w ) / 2; break;
	case POSFROM_HC_B: x = ( WINDOW_W - w ) / 2;
					   y = WINDOW_H - h - y;     break;
	case POSFROM_HO_T: x = 0; w = WINDOW_W;      break;
	case POSFROM_HO_B: x = 0; w = WINDOW_W;
					   y = WINDOW_H - h - y;     break;
	}

	rect.left   = x    ;
	rect.right  = x + w;
	rect.top    = y    ;
	rect.bottom = y + h;

	if( gframe )
	{
		gframe->SetRect( &rect );
		rect.left   += GENERICFRAME_W;
		rect.right  -= GENERICFRAME_W;
		rect.top    += GENERICFRAME_H;
		rect.bottom -= GENERICFRAME_H;
		flags       |= DIALOGUEFLAGS_FRAME;
	}
	return true;
}


void MessageFrame::Clear()
{
	action       = MESSAGEFRAMEACTION_IDLE;
	scroll_y     = 0;
	scan_line    = 0;
	scan_cell_x  = 0;
	char_code    = 0;
	scroll_index = 0;
	cur_index    = 0;

	_b_empty = true;

	for( long l = 0; l < line_max; l++ ) lines[ l ].Clear();
}

MessageFrame::MessageFrame( int num_h, int num_v, POSFROM set, int x, int y )
{

	MessageFrame::flags = 0;

	line_max = line_num = num_v;
	lines    = new MessageLine[ line_max ];

	cell_num = ( FONTWORK_FONTW_W + DIALOGUE_PADDING_X ) * num_h /
			   ( FONTWORK_FONTW_C + DIALOGUE_PADDING_X );

	for( int l = 0; l < line_max; l++ ) lines[ l ].Initialize( cell_num, l );

	gframe = new GenericFrame( NULL );

	MessageFrame::Clear();

	SetPosition( num_h, num_v, set, x, y );
}

MessageFrame::~MessageFrame( void )
{
	if( lines  ) delete[] lines ;
	if( gframe ) delete   gframe;
}

bool MessageFrame::IsEmpty()
{
	return _b_empty;
}


long MessageFrame::SetCell( const unsigned char *p, long size )
{
	unsigned short char_code;

	if( !size || *p == 0        ) return 0;
	if( MessageFrame::char_code ) return 0;

	flags |=  DIALOGUEFLAGS_VISIBLE;

	if( ShiftJIS_Is( (char)*p ) )
	{
		if( size < 2 ) return 0;
		*( (unsigned char*)&char_code + 1 ) = p[ 0 ];
		*( (unsigned char*)&char_code     ) = p[ 1 ];

		MessageFrame::char_code = char_code;
		return 2;
	}
	else
	{
		char_code               = p[ 0 ];
		MessageFrame::char_code = char_code;
		return 1;
	}

	return 0;
}
void MessageFrame::SetBreak( void )
{
	flags |=  DIALOGUEFLAGS_VISIBLE;
	if( scan_line < line_num - 1 ) scan_line++;
	else action = MESSAGEFRAMEACTION_SCROLL;
	scan_cell_x = 0;
}

void MessageFrame::Procedure( void )
{
//	if( flags & DIALOGUEFLAGS_GRAY ) cur_anime = 0;
//	else
	cur_anime++;

	switch( action )
	{
	case MESSAGEFRAMEACTION_IDLE   :
		if( !char_code ) break;

		_b_empty = false;
		scan_cell_x = lines[ scan_line ].SetCell( char_code );
		if( rect.right - rect.left - scan_cell_x < FONTWORK_FONTW_W )
		{
			scan_cell_x = 0;
			if( scan_line == line_num - 1 ) action = MESSAGEFRAMEACTION_SCROLL;
			else                            scan_line++;
		}
		char_code = 0;
		break;

	case MESSAGEFRAMEACTION_SCROLL:

		scroll_y -= DIALOGUE_SCROLL_SPEED;
		if( scroll_y > -DIALOGUE_NEXTLINEOFFSET ) break;

		for( long l = 0; l < line_num - 1; l++ ) lines[ l ].CopyCellsBy( lines[ l + 1 ] );
		lines[ line_num - 1 ].Clear();
		scan_line   = l;
		scroll_y    = 0;
		scan_cell_x = 0;
		action      = MESSAGEFRAMEACTION_IDLE;
		break;
	}
	gframe->Action();
}


void MessageFrame::SetVisible( bool b )
{
	if( b ) flags |=  DIALOGUEFLAGS_VISIBLE;
	else    flags &= ~DIALOGUEFLAGS_VISIBLE;
}

void MessageFrame::Put( const RECT* rc_view )
{
	static const RECT rc_cur[] = {
		{ 224, 96, 232, 108},
		{ 232, 96, 240, 108},
//		{ 240, 96, 248, 108},
//		{ 248, 96, 256, 108},
	};

	long ani_no = ( cur_anime / 8 % 2 ) ? 1 : 0;

	if( !( flags & DIALOGUEFLAGS_VISIBLE ) ) return;

	if( flags & DIALOGUEFLAGS_FRAME && gframe ) gframe->Put( rc_view );

	// cursor

	if( b_cur_visible && action == MESSAGEFRAMEACTION_IDLE )
	{
		DxDraw_Put_Clip( &rect,
			rect.left + scan_cell_x,
			rect.top + scan_line * DIALOGUE_NEXTLINEOFFSET,
			&rc_cur[ ani_no ], SfNo_FONT_1 );
	}

	RECT rc = rect;
	for( int l = 0; l < line_num; l++ ) lines[ l ].Put( &rc, scroll_y );
}


void MessageFrame::Cursor_Visible( bool b )
{
	b_cur_visible = b;
}
