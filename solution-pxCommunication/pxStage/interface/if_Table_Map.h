
void if_Table_Map_Initialize( void );
void if_Table_Map_SetRect( RECT *rcView );
RECT *if_Table_Map_GetRect( void );
void if_Table_Map_SetCursor( int x, int y );
void if_Table_Map_GetCursor( int *px , int *py );
void if_Table_Map_SetSelect( int   x1, int   y1, int   x2, int   y2 );
void if_Table_Map_GetSelect( int *px1, int *py1, int *px2, int *py2 );
void if_Table_Map_VisibleSelect( BOOL b );
void if_Table_Map_Put_Table(  RECT *rcView );
void if_Table_Map_Put_Cursor( RECT *rcView );
void if_Table_Map_Put_Scroll( RECT *rcView );
void if_Table_Map_Put_CursorPosition( int cur_x, int cur_y );
