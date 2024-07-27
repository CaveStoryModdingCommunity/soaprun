#pragma once

#include "../utility_Input/Buttons.h"
#include "GenericFrame.h"


enum MESSAGEFRAMEACTION
{
	MESSAGEFRAMEACTION_IDLE = 0,
	MESSAGEFRAMEACTION_SCROLL,
};



class MessageCell
{
	long           x     ;
	unsigned short code  ;
	
public:

	MessageCell( void );
	MessageCell& MessageCell::operator=( const MessageCell& other ); // ‘ã“ü‰‰ŽZŽq

	void Reset();
	long Set( unsigned short code, long x );
	void Put( const RECT* p_rc, long y );
};

class MessageLine
{
	long          _index     ;
	long          _cell_num  ;
	MessageCell*  _cells     ;
	long          _scan_index;
	long          _scan_x    ;

public:
	
	 MessageLine();
	~MessageLine();

	bool CopyCellsBy( const MessageLine& other );
	void Initialize( long num, long i );
	void Clear();
	long SetCell( unsigned short code );
	void Put( const RECT* p_rc, long scroll_y );
};

class MessageFrame
{
	RECT           rect         ;
	long           flags        ;
	long           cell_num     ;
	long           scroll_y     ;
	long           scroll_index ;
	long           cur_index    ;
	long           line_num     ;
	long           line_max     ;
	MessageLine*   lines        ;
	long           scan_line    ;
	long           scan_cell_x  ;
	unsigned short char_code    ;
	unsigned short cur_anime    ;
	bool           b_cur_visible;

	unsigned short cur_wait    ;

	MESSAGEFRAMEACTION action ;

	GenericFrame*  gframe      ;

	bool           _b_empty    ;

	void _Action_Talk( void );

public:

	 MessageFrame( int num_h, int num_v, POSFROM set, int x, int y );
	~MessageFrame( void );

	bool SetPosition( int num_h, int num_v, POSFROM set, int x, int y );

	int  GetW( void );
	int  GetH( void );

	long SetCell( const unsigned char *p, long size );
	void SetVisible( bool b );

	void Cursor_Visible( bool b );

	bool IsEmpty  ( void );
	void Clear    ( void );
	void SetBreak ( void );
	void Procedure( void );
	void Put      ( const RECT* rc_view );

	int  Rc_left  ( void ){ return rect.left  ; }
	int  Rc_top   ( void ){ return rect.top   ; }
	int  Rc_right ( void ){ return rect.right ; }
	int  Rc_bottom( void ){ return rect.bottom; }

	void SetShock( void ){ if( gframe ) gframe->SetAction( GENERICFRAME_SHOCK ); }
};

