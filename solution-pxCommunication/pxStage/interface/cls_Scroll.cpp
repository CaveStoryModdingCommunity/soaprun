#include <StdAfx.h>

#include "../../Dx/DxDraw.h"
#include "../../Fixture/KeyControl.h"

#include "if_Generic.h"
#include "cls_Scroll.h"

extern long g_Click;
extern long g_ClickTrg;


//////////////////////////////////////////////////////////////
// ■コンストラクタ／デストラクタ ////////////////////////////
//////////////////////////////////////////////////////////////

cls_SCROLL::cls_SCROLL( void )
{
	_act_no       = SCROLLACTION_GRAY;
	_ani_no       = 0;

	_size         = 16;
	_offset       = 0;

	_start_cur    = 0;
	_start_offset = 0;
	_select_move  = SCROLLBUTTONSIZE;
	_body_move    = SCROLLBUTTONSIZE;
	_post_move    = 0;
}

cls_SCROLL::~cls_SCROLL( void )
{
}

//////////////////////////////////////////////////////////////
// ■スタティック関数 ////////////////////////////////////////
//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
// ■cls_SCROLL Privateメンバ関数 ///////////////////////////
//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
// ■cls_SCROLL Public メンバ関数 ///////////////////////////
//////////////////////////////////////////////////////////////


// 初期化
void cls_SCROLL::Initialize( long surf, BOOL bVertical )
{
	_surf      = surf;
	_bVertical = bVertical;
}

// オフセットを設定
void cls_SCROLL::SetOffset( long offset )
{
	_offset = offset;
	if( _offset < 0 ) _offset = 0;
}
long cls_SCROLL::GetOffset( void ){ return _offset; }
long cls_SCROLL::GetSize(   void ){ return _size  ; }

// フィールドの長さを設定
void cls_SCROLL::SetSize( long size )
{
//	//オフセットを再設定
//	if( _size ) _offset = _offset * size / _size;
	_size = size;
}

void cls_SCROLL::SetMove( long cursor_move, long body_move )
{
	_select_move  = cursor_move;
	_body_move    = body_move  ;
}



// ワンタッチスクロール（ move が -2 なら _select_move * -2 移動する）
void cls_SCROLL::Scroll( long view_width, float move )
{
	long max_offset;
	// 水平スクロールバー
	if( !_bVertical ){
		if( view_width < _size ) max_offset = _size - view_width;
		else                     max_offset = 0;

	// 垂直スクロールバー
	}else{
		if( view_width < _size ) max_offset = _size - view_width;
		else                     max_offset = 0;
	}

	_offset += (long)( _select_move * move );
	if( _offset < 0          ) _offset = 0;
	if( _offset > max_offset ) _offset = max_offset;
}

void cls_SCROLL::PostMove( float move )
{
	if( _act_no == SCROLLACTION_ENABLE ) _post_move = move;
	else                                 _post_move = 0;
}

void cls_SCROLL::SetRect( const RECT *p_field_rect )
{
	long view_width;
	long max_offset;
	long knob_width;
	long knob_offset;

	// 水平スクロールバー
	if( !_bVertical )
	{
		view_width = p_field_rect->right - p_field_rect->left;

		if( view_width < _size )
		{
			max_offset = _size - view_width;
		}
		else
		{
			_offset    = 0;
			max_offset = 0;
		}

		if( _size ){
			knob_width  = view_width * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
			knob_offset = _offset    * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
		}else{
			knob_width  = 0;
			knob_offset = 0;
		}

		_rcBtn1.left   = p_field_rect->left;
		_rcBtn1.right  = p_field_rect->left   + SCROLLBUTTONSIZE;
		_rcBtn1.top    = p_field_rect->bottom;
		_rcBtn1.bottom = p_field_rect->bottom + SCROLLBUTTONSIZE;

		_rcBtn2.left   = p_field_rect->right  - SCROLLBUTTONSIZE;
		_rcBtn2.right  = p_field_rect->right;
		_rcBtn2.top    = p_field_rect->bottom;
		_rcBtn2.bottom = p_field_rect->bottom + SCROLLBUTTONSIZE;

		_rcKnob.left   = p_field_rect->left   + SCROLLBUTTONSIZE + knob_offset              -1;
		_rcKnob.right  = p_field_rect->left   + SCROLLBUTTONSIZE + knob_offset + knob_width +1;
		_rcKnob.top    = p_field_rect->bottom;
		_rcKnob.bottom = p_field_rect->bottom + SCROLLBUTTONSIZE;


	// 垂直スクロールバー
	}
	else
	{
		view_width = p_field_rect->bottom - p_field_rect->top;

		if( view_width < _size )
		{
			max_offset = _size - view_width;
		}
		else
		{
			_offset    = 0;
			max_offset = 0;
		}

		if( _size )
		{
			knob_width  = view_width * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
			knob_offset = _offset    * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
		}
		else
		{
			knob_width  = 0;
			knob_offset = 0;
		}

		_rcBtn1.left   = p_field_rect->right;
		_rcBtn1.right  = p_field_rect->right  + SCROLLBUTTONSIZE;
		_rcBtn1.top    = p_field_rect->top;
		_rcBtn1.bottom = p_field_rect->top    + SCROLLBUTTONSIZE;

		_rcBtn2.left   = p_field_rect->right;
		_rcBtn2.right  = p_field_rect->right  + SCROLLBUTTONSIZE;
		_rcBtn2.top    = p_field_rect->bottom - SCROLLBUTTONSIZE;
		_rcBtn2.bottom = p_field_rect->bottom;

		_rcKnob.left   = p_field_rect->right;
		_rcKnob.right  = p_field_rect->right  + SCROLLBUTTONSIZE;
		_rcKnob.top    = p_field_rect->top    + SCROLLBUTTONSIZE + knob_offset              -1;
		_rcKnob.bottom = p_field_rect->top    + SCROLLBUTTONSIZE + knob_offset + knob_width +1;

	}

}

// アクション
BOOL cls_SCROLL::Action( const RECT *p_field_rect, const POINT* p_cur )
{
	long view_width;
	long max_offset;
	long knob_width;
	long knob_offset;

	static _counter;

	// 水平スクロールバー
	if( !_bVertical )
	{
		view_width = p_field_rect->right - p_field_rect->left;

		if( view_width < _size )
		{
			max_offset = _size - view_width;
			if( p_field_rect->right - p_field_rect->left > 2 * SCROLLBUTTONSIZE )
				Scroll( p_field_rect->right  - p_field_rect->left, _post_move );
		}
		else
		{
			_offset    = 0;
			max_offset = 0;
		}
		_post_move = 0;

		if( _size )
		{
			knob_width  = view_width * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
			knob_offset = _offset    * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
		}
		else
		{
			knob_width  = 0;
			knob_offset = 0;
		}

		if( p_field_rect->right - p_field_rect->left <= 2 * SCROLLBUTTONSIZE ) return FALSE;

	// 垂直スクロールバー
	}
	else
	{

		view_width = p_field_rect->bottom - p_field_rect->top;

		if( view_width < _size ){
			max_offset = _size - view_width;
			if( p_field_rect->bottom - p_field_rect->top > 2 * SCROLLBUTTONSIZE )
				Scroll( p_field_rect->bottom - p_field_rect->top, _post_move );
		}else{
			_offset    = 0;
			max_offset = 0;
		}
		_post_move = 0;

		if( _size ){
			knob_width  = view_width * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
			knob_offset = _offset    * (view_width - SCROLLBUTTONSIZE*2 ) / _size;
		}else{
			knob_width  = 0;
			knob_offset = 0;
		}
		if( p_field_rect->bottom - p_field_rect->top <= 2 * SCROLLBUTTONSIZE ) return FALSE;

	}

	if( !max_offset ){
		_ani_no = 0;
		_act_no = SCROLLACTION_GRAY;
		return FALSE;
	}

	BOOL bReturn = FALSE;

	switch( _act_no ){
	// 無効
	case SCROLLACTION_GRAY:
		_ani_no = 0;
		if( view_width < _size ) _act_no = SCROLLACTION_ENABLE;
		else break;

	// 有効
	case SCROLLACTION_ENABLE:
		_ani_no = 1;
		if( view_width >= _size ){
			_act_no   = SCROLLACTION_GRAY;
			_offset = 0;
			break;
		}
		// クリック／ドラッグ
		if( KeyControl_IsClickLeftTrigger() ){


			if( !knob_width ) knob_width = 1;
			// ←
			if(       p_cur->x >= _rcBtn1.left  && p_cur->x <  _rcBtn1.right  &&
				      p_cur->y >= _rcBtn1.top   && p_cur->y <  _rcBtn1.bottom ){
				_act_no   = SCROLLACTION_LEFT;
				_ani_no   =  2;
				_offset  -= _select_move;
				_counter  =  0;
				bReturn   = TRUE;
			// →
			}else if( p_cur->x >= _rcBtn2.left  && p_cur->x <  _rcBtn2.right  &&
					  p_cur->y >= _rcBtn2.top   && p_cur->y <  _rcBtn2.bottom ){
				_act_no    = SCROLLACTION_RIGHT;
				_ani_no    =  2;
				_offset += _select_move;
				_counter          =  0;
				bReturn   = TRUE;
			// Drag
			}else if( p_cur->x >= _rcKnob.left  && p_cur->x <  _rcKnob.right  &&
					  p_cur->y >= _rcKnob.top   && p_cur->y <  _rcKnob.bottom ){
				_ani_no         =  2;
				_act_no         = SCROLLACTION_DRAG;
				_start_offset   = _offset;
				if( !_bVertical ) _start_cur = p_cur->x;
				else              _start_cur = p_cur->y;
				bReturn   = TRUE;
			// click
			}else if( p_cur->x >= _rcBtn1.left   &&
					  p_cur->x <  _rcBtn2.right  &&
					  p_cur->y >= _rcBtn1.top    &&
					  p_cur->y <  _rcBtn2.bottom ){
				_ani_no    =  2;
				if( !_bVertical )
				{
					if( p_cur->x <= _rcKnob.left ) _offset -= _body_move;
					else                        _offset += _body_move;
				}
				else
				{
					if( p_cur->y <= _rcKnob.top  ) _offset -= _body_move;
					else                        _offset += _body_move;
				}
				bReturn   = TRUE;
			}

		}
		break;

	// ドラッグ中
	case SCROLLACTION_DRAG:
		bReturn   = TRUE;
		if( KeyControl_IsClickLeft() ){
			long drag;
			if( !_bVertical ) drag = ( p_cur->x - _start_cur ) * _size / (view_width - 32);
			else              drag = ( p_cur->y - _start_cur ) * _size / (view_width - 32);
			_offset = _start_offset + drag;
		}else{
			if( view_width >= _size ) _act_no = SCROLLACTION_GRAY;
			else                      _act_no = SCROLLACTION_ENABLE;
		}
		break;
	// ←押し
	case SCROLLACTION_LEFT:
		bReturn   = TRUE;
		if( KeyControl_IsClickLeft() &&
			p_cur->x >= _rcBtn1.left  && p_cur->x <  _rcBtn1.right  &&
			p_cur->y >= _rcBtn1.top   && p_cur->y <  _rcBtn1.bottom ){

			_counter++;
			if( _counter > 20 ){
				_counter = 18;
				_offset -= _select_move;
			}
	
		}else{
			if( view_width >= _size ) _act_no = SCROLLACTION_GRAY;
			else                      _act_no = SCROLLACTION_ENABLE;
		}
		break;
	// →押し
	case SCROLLACTION_RIGHT:
		bReturn   = TRUE;
		if( KeyControl_IsClickLeft() &&
			p_cur->x >= _rcBtn2.left  && p_cur->x <  _rcBtn2.right  &&
			p_cur->y >= _rcBtn2.top   && p_cur->y <  _rcBtn2.bottom ){

			_counter++;
			if( _counter > 20 ){
				_counter = 18;
				_offset += _select_move;
			}
	
		}else{
			if( view_width >= _size ) _act_no = SCROLLACTION_GRAY;
			else                      _act_no = SCROLLACTION_ENABLE;
		}
		break;
	}

	if( _offset < 0          ) _offset = 0;
	if( _offset > max_offset ) _offset = max_offset;

	_offset = _offset;

	return bReturn;
}


void cls_SCROLL::Put( const RECT *rcField )
{
	RECT rcItem;

	RECT rcLeft[] = {
		{  0,  0, 16, 16},
		{  0, 16, 16, 32},
		{  0, 32, 16, 48},
	};
	RECT rcRight[] = {
		{ 16,  0, 32, 16},
		{ 16, 16, 32, 32},
		{ 16, 32, 32, 48},
	};
	RECT rcBodyH = { 48,  0,176, 16};
	RECT rcKnobH[] = {
		{  0,  0,  0,  0},
		{ 48, 16,176, 32},
		{ 48, 32,176, 48},
	};
	RECT rcBodyV = {  0, 48, 16,176};
	RECT rcKnobV[] = {
		{  0,  0,  0,  0},
		{ 16, 48, 32,176},
		{ 32, 48, 48,176},
	};


	// 水平
	if( !_bVertical ){
		rcItem.left   = _rcBtn1.right;
		rcItem.right  = _rcBtn2.left;
		rcItem.top    = _rcBtn1.top;
		rcItem.bottom = _rcBtn1.bottom;
		Tiling_Horizontal( &rcItem, &rcBodyH,          1, 0, _surf );

		rcItem   = _rcKnob;
		rcItem.left  += 1;
		rcItem.right -= 1;
		if( rcItem.right <= rcItem.left ) rcItem.right = rcItem.left + 1;
		Tiling_Horizontal( &rcItem, &rcKnobH[_ani_no], 1, 0, _surf );

	// 垂直
	}else{
		rcItem.left   = _rcBtn1.left;
		rcItem.right  = _rcBtn1.right;
		rcItem.top    = _rcBtn1.bottom;
		rcItem.bottom = _rcBtn2.top;
		Tiling_Vertical(   &rcItem, &rcBodyV,          1, 0, _surf );
		rcItem   = _rcKnob;
		rcItem.top    += 1;
		rcItem.bottom -= 1;
		if( rcItem.bottom <= rcItem.top ) rcItem.bottom = rcItem.top + 1;
		Tiling_Vertical(   &rcItem, &rcKnobV[_ani_no], 1, 0, _surf );

	}
	DxDraw_Put( _rcBtn1.left, _rcBtn1.top, &rcLeft[ _ani_no], _surf );
	DxDraw_Put( _rcBtn2.left, _rcBtn2.top, &rcRight[_ani_no], _surf );

}