#include "cls_Scroll.h"

// カーソルアクション
enum enum_CursorAction{
	enum_CursorAction_Free = 0,
	enum_CursorAction_Drag_1,
	enum_CursorAction_Drag_2,
	enum_CursorAction_Drag_3,
};

enum enum_CursorFocus
{
	enum_CursorFocus_None = 0,
	enum_CursorFocus_Tools,
	enum_CursorFocus_Palette,
	enum_CursorFocus_Map,
//	enum_CursorFocus_FieldDoor,
	enum_CursorFocus_FldObj,
	enum_CursorFocus_Attribute,
	enum_CursorFocus_MapH,
	enum_CursorFocus_MapV,
	enum_CursorFocus_AttH,
	enum_CursorFocus_AttV,
};


typedef struct
{
	RECT              *p_field_rect;
	enum_CursorFocus  focus;
	enum_CursorAction action;
	int               target;
	int               drag_xpos;
	int               drag_ypos;
	int               scroll_count;
	int               start_x;
	int               start_y;
	DWORD             wait_count;

}ACTIONCURSOR;

extern ACTIONCURSOR g_Cursor;

void if_Cursor_DragScroll( int start, int end, int cur, cls_SCROLL* scrl );
void if_Cursor_Initialize( void );
bool if_Cursor_Action( const POINT* p_cur, bool bKeyboard );
void if_Cursor_Put(  void );


