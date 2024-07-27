#pragma once

#include "SimpleList.h"
#include "../utility_Input/Buttons.h"
#include "GenericFrame.h"

enum LISTFRAMEMODE
{
	LISTFRAME_LIST = 0,
	LISTFRAME_SUBLIST ,
	LISTFRAME_STATIC  ,
};

// ListCell Class..
class ListCell
{
private:

	unsigned short _x   ;
	unsigned short _code;
	
public:

	ListCell( void );
	ListCell& ListCell::operator=( const ListCell& other ); // 代入演算子

	void Reset();
	long Set( unsigned short code, unsigned short x );
	int  SetRight( int scan_x );
	void Put( const RECT* p_rc, int y, bool b_dark );
};

// ListLine Class..
class ListLine
{
private:

	short     _index   ;
	short     _cell_num;
	ListCell* _cells   ;
	bool      _b_dark  ;

public:
	
	 ListLine();
	~ListLine();

	bool CopyCellsBy( const ListLine& other );
	void Initialize( short num, short index );
	void Clear();
	int  SetText( const char* text, bool b_proportion, bool b_dark );
	void SetRight( int frame_w );
	void Put( const RECT* p_rc, int scroll_y );
	unsigned short GetCellX( int cell_index );
};

// ListFrame Class..
class ListFrame
{
private:

	enum _LISTFRAMEACTION
	{
		_LISTFRAMEACTION_IDLE = 0,
		_LISTFRAMEACTION_SCROLL,
	};

	RECT              _rect         ;
	long              _scroll_y     ;
	long              _scroll_index ;
	long              _indent_w     ;
	short             _cur_y        ;
	short             _cur_x        ;
	short             _cur_y_old    ;
	unsigned short    _line_num     ;
	unsigned short    _line_max     ;
	ListLine*         _lines        ;
	unsigned short    _cur_anime    ;
	unsigned short    _cur_wait     ;

	bool              _b_visible    ;
	bool              _b_frame      ;
	bool              _b_cur_visible;
	bool              _b_right      ;
	bool              _b_cur_x      ;
	bool              _b_propotion  ;

	bool              _b_hold       ;
	bool              _b_hold_move  ;
	unsigned long     _hold_color   ;

	unsigned short    _num_h        ; // コンストラクタで受け取った値
	unsigned short    _list_num     ;
	const SIMPLELIST* _lists        ;

	LISTFRAMEMODE     _mode         ;
	_LISTFRAMEACTION  _act          ;
								
	GenericFrame*     _frame        ;

	bool  _Action_List( const BUTTONSSTRUCT* p_btns, bool b_mute );
	short _Set_CursorY( short cur_y ); // 内部用

public:

	 ListFrame( LISTFRAMEMODE mode, int num_h, int num_v, POSFROM set, int x, int y, bool bFrame );
	~ListFrame( void );

	bool SetPosition( int num_h, int num_v, POSFROM set, int x, int y );

	int  GetW( void );
	int  GetH( void );

	void SetVisible   ( bool b );
	void SetProportion( bool b );

	bool List_Set( const SIMPLELIST* pList, unsigned short num );
	bool List_Set( const SIMPLELIST* pList, unsigned short num, bool b_right );
	void List_Reset( void );
	long List_Set_CursorY( short cur_y  );
	void List_Set_CursorFix     ( void    );
	long List_Get_CursorY       ( void    );
	long List_Get_CursorY_old   ( void    );
	long List_Get_CursorParam   ( void    );
	bool List_DarkBeep          ( void    );
	
	int  List_CursorHold        ( void    );
	bool List_CursorFree_IsMoved( void    );


	void  Cursor_Visible      ( bool b  );
	void  Cursor_Set_Move_X   ( bool b  );
	short Cursor_Get_X        ( void    );
	void  Cursor_Set_X        ( short x );
	void  Cursor_Set_X_Last   ( void    );
	bool  Cursor_Set_Find     ( unsigned short code );

 
	void Clear(    void );
	bool Action(   const BUTTONSSTRUCT* p_btns );
	void Put(      const RECT* rc_veiw );

	int  Rc_L( void ){ return _rect.left  ; }
	int  Rc_T( void ){ return _rect.top   ; }
	int  Rc_R( void ){ return _rect.right ; }
	int  Rc_B( void ){ return _rect.bottom; }
};

// #endif
