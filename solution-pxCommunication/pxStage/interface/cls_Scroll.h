#pragma once

#define SCROLLBUTTONSIZE 16

enum{
	SCROLLACTION_GRAY,
	SCROLLACTION_ENABLE,
	SCROLLACTION_DRAG,
	SCROLLACTION_LEFT,
	SCROLLACTION_RIGHT,
};

class cls_SCROLL{

private:
	long _surf;
	BOOL _bVertical;

	long _act_no;
	long _ani_no;

	long _size;
	long _offset;

	long _start_cur;
	long _start_offset;
	long _select_move; //矢印移動量
	long _body_move;   //ボディ移動量
	float _post_move;   //マウスホイール用

	RECT _rcBtn1;
	RECT _rcBtn2;
	RECT _rcKnob;

	cls_SCROLL(              const cls_SCROLL &src  ){               } // copy
	cls_SCROLL & operator = (const cls_SCROLL &right){ return *this; } // substitution

public:
	 cls_SCROLL( void );
	~cls_SCROLL( void );

	void Initialize( long surf, BOOL bVertical );
	void SetSize(   long size   );
	void SetOffset( long offset );
	long GetOffset( void );
	long GetSize(   void );
	void SetMove( long cursor_move, long body_move );
	void SetRect( const RECT *p_field_rect );
	BOOL Action(  const RECT *p_field_rect, const POINT* p_cur );
	void Put(     const RECT *p_field_rect );
	void Scroll( long view_width, float move );
	void PostMove( float move );

};
