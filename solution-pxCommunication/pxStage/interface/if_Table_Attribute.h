void if_Table_Attribute_Initialize( void );
void if_Table_Attribute_SetRect( RECT *rcView );
RECT *if_Table_Attribute_GetRect( void );
void if_Table_Attribute_SetCursor( int    x, int y );
void if_Table_Attribute_GetCursor( int *p_x, int *p_y );
void if_Table_Attribute_SetSelect( int   x1, int y1, int x2, int y2 );
void if_Table_Attribute_GetSelect( int *px1, int *py1, int *px2, int *py2 );
void if_Table_Attribute_VisibleSelect( BOOL b );
void if_Table_Attribute_Put_Table(  RECT *rcView );
void if_Table_Attribute_Put_Cursor( RECT *rcView );
void if_Table_Attribute_Put_Scroll( RECT *rcView );
void if_Table_Attribute_Put_CursorPosition( int cur_x, int cur_y );