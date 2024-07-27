#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
#include "../../Fixture/ShiftJIS.h"

#include "../utility_Sound/SEFF.h"

#include "ListFrame.h"
#include "FontWork.h"

#define DIALOGUE_NEXTLINEOFFSET (FONTWORK_FONTH + DIALOGUE_PADDING_Y)

// コンストラクタ
ListCell::ListCell( void ){ ListCell::Reset(); }

// 代入演算子
ListCell& ListCell::operator=( const ListCell& other )
{
	if( this != &other )
	{
		_x     = other._x    ;
		_code  = other._code ;
	}
	return *this;
}

void ListCell::Reset()
{
	_x     = 0;
	_code  = 0;
}

long ListCell::Set( unsigned short code, unsigned short x )
{
	ListCell::_code = code;
	ListCell::_x    = x   ;
	return FontWork_GetWidth( code );
}

int ListCell::SetRight( int scan_x )
{
	if( !_code ) return scan_x;
	scan_x -= FontWork_GetWidth( _code ) + DIALOGUE_PADDING_X;
	ListCell::_x    = scan_x;
	return scan_x;
}

void ListCell::Put( const RECT* p_rc, int y, bool b_dark )
{
	if( _code ) FontWork_Put( p_rc, p_rc->left + _x, y, _code, b_dark ? FONTFLAG_DARK : 0 );
}

// ListLine ##################################

// コンストラクタ
ListLine::ListLine()
{
	_cells    = NULL ;
	_cell_num =     0;
	_b_dark   = false;
}

// デストラクタ
ListLine::~ListLine()
{
	if( _cells ) delete[] _cells;
	_cell_num = 0;
}

bool ListLine::CopyCellsBy( const ListLine& other )
{
	if( this == &other               ) return false;
	if( other._cell_num != _cell_num ) return false;
	for( long c = 0; c < _cell_num; c++ ) _cells[ c ] = other._cells[ c ];
	return true;
}

void ListLine::Initialize( short num, short index )
{
	_cells    = new ListCell[ num ];
	_cell_num = num  ;
	_index    = index;
}

void ListLine::Clear()
{
	for( long c = 0; c < _cell_num; c++ ) _cells[ c ].Reset();
}

int ListLine::SetText( const char* text, bool b_proportion, bool b_dark )
{
	unsigned short char_code;

	int c = 0;
	int x = 0;
	int w;

	_b_dark = b_dark;

	for( int i = 0;  text[ i ] != '\0'; i++ )
	{
		if( ShiftJIS_Is( text[ i ] ) )
		{
			*( (unsigned char*)&char_code + 1 ) = text[ i ];
			i++; if( text[ i ] == '\0' ) return 0;
			*( (unsigned char*)&char_code     ) = text[ i ];
		}
		else
		{
			char_code = (unsigned char)text[ i ];
		}
		w = _cells[ c ].Set( char_code, x );
		if( b_proportion ) x += w                + DIALOGUE_PADDING_X;
		else               x += FONTWORK_FONTW_W + DIALOGUE_PADDING_X;
		c++;
		if( c >= _cell_num ) break;
	}

	for( c; c < _cell_num; c++ ) _cells[ c ].Reset();

	return x;
}

void ListLine::SetRight( int frame_w )
{
	int scan_x = frame_w;
	for( int c = _cell_num - 1; c >= 0; c-- ) scan_x = _cells[ c ].SetRight( scan_x );
}

void ListLine::Put( const RECT* p_rc, int scroll_y )
{
	for( int c = 0; c < _cell_num; c++ )
	{
		_cells[ c ].Put( p_rc, p_rc->top + scroll_y + _index * DIALOGUE_NEXTLINEOFFSET, _b_dark );
	}
}


// ListFrame ##################################

bool ListFrame::SetPosition( int num_h, int num_v, POSFROM set, int x, int y )
{
	if( num_v > _line_max ) return false;
	if( num_v <         1 ) return false;
	_line_num = num_v;

	// 必要はば
	long w, h;
	w = _indent_w + ( FONTWORK_FONTW_W + DIALOGUE_PADDING_X ) * num_h - DIALOGUE_PADDING_X + ( _frame ? (GENERICFRAME_W * 2) : 0 );
	h =             ( FONTWORK_FONTH   + DIALOGUE_PADDING_Y ) * num_v - DIALOGUE_PADDING_Y + ( _frame ? (GENERICFRAME_H * 2) : 0 );

	switch( set )
	{
	case POSFROM_LT  :                             break;
	case POSFROM_RT  : x = WINDOW_W - w - x;     break;
	case POSFROM_LB  : y = WINDOW_H - h - y;     break;
	case POSFROM_RB  : x = WINDOW_W - w - x;
					   y = WINDOW_H - h - y;     break;
	case POSFROM_HC_T: x += ( WINDOW_W - w ) / 2; break;
	case POSFROM_HC_B: x += ( WINDOW_W - w ) / 2;
					   y = WINDOW_H - h - y;     break;
	case POSFROM_HO_T: x = 0; w = WINDOW_W;      break;
	case POSFROM_HO_B: x = 0; w = WINDOW_W;
					   y = WINDOW_H - h - y;     break;
	}

	_rect.left   = x    ;
	_rect.right  = x + w;
	_rect.top    = y    ;
	_rect.bottom = y + h;

	if( _frame )
	{
		_frame->SetRect( &_rect );
		_rect.left   += GENERICFRAME_W;
		_rect.right  -= GENERICFRAME_W;
		_rect.top    += GENERICFRAME_H;
		_rect.bottom -= GENERICFRAME_H;
		_b_frame      = true;
	}
	return true;
}

ListFrame::ListFrame( LISTFRAMEMODE mode, int num_h, int num_v, POSFROM set, int x, int y, bool bFrame )
{
	_mode         = mode ;
	_act          = _LISTFRAMEACTION_IDLE;

	_lists        = NULL ;
	_list_num     =     0;

	_scroll_y     =     0;
	_scroll_index =     0;

	_cur_y        =     0;
	_cur_x        =     0;
	_cur_wait     =     0;
	_cur_anime    =     0;

	_b_cur_x      = false;
	_b_propotion  = true ;
	_b_visible    = false;
	_b_frame      = false;
	_b_right      = false;
	_b_hold       = false;

	_hold_color   = DxDraw_GetSurfaceColor( SfNo_FRAME_M );

	// 文字
	switch( _mode )
	{
	case LISTFRAME_LIST: _indent_w = DIALOGUE_LISTINDENT_W; _b_cur_visible = TRUE ; break;
	default                : _indent_w =                     0; _b_cur_visible = FALSE; break;
	}

	_num_h    =             num_h;
	_line_max = _line_num = num_v;
	_lines    = new ListLine[ _line_max ];

	int cell_num = ( FONTWORK_FONTW_W + DIALOGUE_PADDING_X ) * num_h /
				   ( FONTWORK_FONTW_C + DIALOGUE_PADDING_X );

	for( int l = 0; l < _line_max; l++ ) _lines[ l ].Initialize( cell_num, l );

	if( bFrame ) _frame = new GenericFrame( NULL );
	else         _frame = NULL;

	SetPosition( num_h, num_v, set, x, y );
}

ListFrame::~ListFrame( void )
{
	if( _lines ) delete[] _lines;
	if( _frame ) delete   _frame;
}

void ListFrame::Clear()
{
	_act          = _LISTFRAMEACTION_IDLE;
	_scroll_y     = 0;
	_scroll_index = 0;
	_cur_y        = 0;

	for( int l = 0; l < _line_max; l++ ) _lines[ l ].Clear();
}

bool ListFrame::List_Set( const SIMPLELIST* pList, unsigned short num, bool b_right )
{
	if( !( _list_num = num ) ) return false;

	_b_visible = true;
	_lists     = pList;
	_b_right   = b_right;

	List_Set_CursorY( 0 );
	return true;
}
bool ListFrame::List_Set( const SIMPLELIST* pList, unsigned short num )
{
	return List_Set( pList, num, false );
}

bool ListFrame::_Action_List( const BUTTONSSTRUCT* p_btns, bool b_mute )
{
	_cur_anime++;

	bool b_cur_move = false;

	_cur_y_old      = _cur_y;

	switch( _act )
	{
	case _LISTFRAMEACTION_IDLE   :

		long old_scroll_index;

		old_scroll_index = _scroll_index;

		if     (             p_btns->trg & KEY_BUTTON_DOWN  ){                          _cur_wait =  0; _cur_y++; b_cur_move = true;   }
		else if(             p_btns->trg & KEY_BUTTON_UP    ){                          _cur_wait =  0; _cur_y--; b_cur_move = true;   }
		else if( _b_cur_x && p_btns->trg & KEY_BUTTON_RIGHT ){                          _cur_wait =  0; _cur_x++; b_cur_move = true;   }
		else if( _b_cur_x && p_btns->trg & KEY_BUTTON_LEFT  ){                          _cur_wait =  0; _cur_x--; b_cur_move = true;   }
		else if(             p_btns->btn & KEY_BUTTON_DOWN  ){ if( ++_cur_wait >= 20 ){ _cur_wait = 16; _cur_y++; b_cur_move = true; } }
		else if(             p_btns->btn & KEY_BUTTON_UP    ){ if( ++_cur_wait >= 20 ){ _cur_wait = 16; _cur_y--; b_cur_move = true; } }
		else if( _b_cur_x && p_btns->btn & KEY_BUTTON_RIGHT ){ if( ++_cur_wait >= 20 ){ _cur_wait = 16; _cur_x++; b_cur_move = true; } }
		else if( _b_cur_x && p_btns->btn & KEY_BUTTON_LEFT  ){ if( ++_cur_wait >= 20 ){ _cur_wait = 16; _cur_x--; b_cur_move = true; } }
		else _cur_wait = 0;

		_Set_CursorY( _cur_y );

		if( _b_hold && _cur_y != _cur_y_old ) _b_hold_move = true;

		if     ( _scroll_index < old_scroll_index ){ _act = _LISTFRAMEACTION_SCROLL; _scroll_y = -DIALOGUE_NEXTLINEOFFSET; }
		else if( _scroll_index > old_scroll_index ){ _act = _LISTFRAMEACTION_SCROLL; _scroll_y = +DIALOGUE_NEXTLINEOFFSET; }

		if( _cur_x <       0 ){ _cur_x = -1; _b_cur_visible = false; }
		if( _cur_x >= _num_h ){ _cur_x = -2; _b_cur_visible = false; }

		break;

	case _LISTFRAMEACTION_SCROLL:

		if( _scroll_y < 0 ) _scroll_y += DIALOGUE_SCROLL_SPEED   ;
		if( _scroll_y > 0 ) _scroll_y -= DIALOGUE_SCROLL_SPEED   ;
		if( _scroll_y <  DIALOGUE_SCROLL_SPEED &&
			_scroll_y > -DIALOGUE_SCROLL_SPEED )
		{
			_scroll_y = 0;
			_act      = _LISTFRAMEACTION_IDLE;
		}

		if(      p_btns->trg & KEY_BUTTON_DOWN ) _cur_wait = 0;
		else if( p_btns->trg & KEY_BUTTON_UP   ) _cur_wait = 0;
		else if( p_btns->btn & KEY_BUTTON_DOWN ) _cur_wait++;
		else if( p_btns->btn & KEY_BUTTON_UP   ) _cur_wait++;
		else _cur_wait = 0;

		break;
	}

	if( b_cur_move )
	{
		if( _b_cur_visible ) SEFF_Voice_Play( /*_b_hold ? SeNo_ITEM_MOVE :*/ SeNo_Select );
		return true;
	}
	return false;
}

bool ListFrame::Action( const BUTTONSSTRUCT* p_btns )
{
	switch( _mode )
	{
	case LISTFRAME_LIST   : return _Action_List( p_btns, false );
	case LISTFRAME_SUBLIST: return _Action_List( p_btns, true  );
	}
	return false;
}

void ListFrame::SetVisible   ( bool b ){ _b_visible   = b; }
void ListFrame::SetProportion( bool b ){ _b_propotion = b; }

void ListFrame::List_Reset( void )
{
	const char*       p  ;
	const SIMPLELIST* lst;
	for( int l = 0; l < _line_num; l++ ) _lines[ l ].Clear();
	for( int l = 0; l < _list_num && l < _line_num; l++ )
	{
		lst = &_lists[ l + _scroll_index ];
		if(      lst->p_buf ) p = lst->p_buf;
		else if( lst->p_str ) p = lst->p_str;
		else break;

		//if( !
			_lines[ l ].SetText( p, _b_propotion, lst->b_dark );
			//) break;

	}
	if( _b_right )
	{
		int frame_w = _rect.right - _rect.left;
		for( int l = 0; l < _line_num; l++ ) _lines[ l ].SetRight( frame_w );
	}
}

short ListFrame::_Set_CursorY( short cur_y )
{
	_b_visible = true;

	// cur_y
	if(      cur_y >= _list_num                     ) cur_y = 0;
	else if( cur_y <          0                     ) cur_y = _list_num - 1;

	// scroll_index
	if(      _scroll_index > cur_y                  ) _scroll_index = cur_y;
	else if( _scroll_index < cur_y  - _line_num + 1 ) _scroll_index = cur_y  - _line_num + 1;

	_cur_y = cur_y;
	List_Reset();

	return cur_y;
}

long ListFrame::List_Set_CursorY( short cur_y )
{
	_cur_y_old = _Set_CursorY( cur_y );
	return _cur_y_old;
}

long ListFrame::List_Get_CursorY    ( void ){ return _cur_y    ; }
long ListFrame::List_Get_CursorY_old( void ){ return _cur_y_old; }

long ListFrame::List_Get_CursorParam( void )
{
	return _lists[ _cur_y ].param;
}

bool ListFrame::List_DarkBeep( void )
{
	if( _lists[ _cur_y ].b_dark )
	{
		SEFF_Voice_Play( SeNo_Error );
		return true;
	}
	return false;
}

int  ListFrame::List_CursorHold( void )
{
	_b_hold      = true ;
	_b_hold_move = false;
//	if( _b_cur_visible ) SEFF_Voice_Play( SeNo_ITEM_HOLD );
	return _cur_y;
}

bool  ListFrame::List_CursorFree_IsMoved( void )
{
	_b_hold      = false;
	return _b_hold_move;
}


void ListFrame::List_Set_CursorFix( void )
{
	_scroll_y  = 0;
	_cur_anime = 0;
}

void ListFrame::Put( const RECT* rc_view )
{
	static const RECT rc_cur[] = // →
	{
		{ 240, 96, 248, 108},
		{ 248, 96, 256, 108},
	};

	static const RECT rc_updown[] =
	{
		{ 240, 108, 248, 120},
		{ 248, 108, 256, 120},
	};

	static const RECT rc_scroll[] = 
	{
		{216, 96,224,100},
		{216,100,224,104},
	};

	static const RECT rc_beem[] =
	{
		{224,108,232,120},
		{232,108,240,120},
	};

	const RECT* rcs;


	if( !_b_visible        ) return;
	if( _b_frame && _frame ) _frame->Put( rc_view );

	// cursor
	if( _act == _LISTFRAMEACTION_IDLE )
	{

		int ani_no = ( _cur_anime / 8 % 2 ) ? 1 : 0;
		int x, y;
		RECT rc_hold;

		switch( _mode )
		{
		case LISTFRAME_LIST   :
			if(_b_hold ) rcs = rc_updown;
			else         rcs = rc_cur   ;
			x              = _rect.left;
			rc_hold.left   = _rect.left + FONTWORK_FONTW_W;
			rc_hold.right  = _rect.right;
			break;
		case LISTFRAME_SUBLIST:
			rcs            = rc_beem;
			x              = _rect.left + _cur_x * ( DIALOGUE_PADDING_X + FONTWORK_FONTW_W );
			rc_hold.left   = _rect.left;
			rc_hold.right  = _rect.right + 1;
			break;
		}
		y = _rect.top + ( _cur_y - _scroll_index ) * DIALOGUE_NEXTLINEOFFSET;

		if( _b_hold )
		{
			rc_hold.top    = y;
			rc_hold.bottom = y + FONTWORK_FONTH + DIALOGUE_PADDING_Y;
			DxDraw_PaintRect( &rc_hold, _hold_color );
		}
		if( _b_cur_visible )
		{
			DxDraw_Put_Clip( &_rect, x, y, &rcs[ ani_no ], SfNo_FONT_1 );
			if( _scroll_index                         ) DxDraw_Put( _rect.left, _rect.top    - 1, &rc_scroll[ 0 ], SfNo_FONT_1 );
			if( _scroll_index < _list_num - _line_num ) DxDraw_Put( _rect.left, _rect.bottom - 1, &rc_scroll[ 1 ], SfNo_FONT_1 );
		}
	}

	RECT rc = _rect;
	rc.left += _indent_w;
	for( int l = 0; l < _line_num; l++ )
		_lines[ l ].Put( &rc, _scroll_y );
}

void  ListFrame::Cursor_Visible   ( bool b  ){ _b_cur_visible = b;  }
void  ListFrame::Cursor_Set_Move_X( bool b  ){ _b_cur_x       = b;  }
short ListFrame::Cursor_Get_X     ( void    ){ return _cur_x;       }
void  ListFrame::Cursor_Set_X     ( short x ){ _cur_x = x;          _cur_wait = 0; _b_cur_visible = true; }
void  ListFrame::Cursor_Set_X_Last( void    ){ _cur_x = _num_h - 1; _cur_wait = 0; _b_cur_visible = true; }

bool  ListFrame::Cursor_Set_Find  ( unsigned short code )
{
	unsigned char* p ;
	unsigned short co;
	short          x ;

	for( int y = 0; y < _list_num; y++ )
	{
		if( p = (unsigned char*)_lists[ y ].p_buf )
		{
			x = 0;
			while( x < _num_h && *p )
			{
				co = p[ 0 ];
				if( *p & 0x80 ) co += ( (unsigned short)p[ 1 ] << 8 );
				if( co == code )
				{
					_cur_x         = x   ;
					_cur_y         = y   ;
					_b_cur_visible = true;
					return true;
				}
				if( *p & 0x80 ) p += 2;
				else            p++   ;
				x++;
			}
		}
	}
	return false;
}


