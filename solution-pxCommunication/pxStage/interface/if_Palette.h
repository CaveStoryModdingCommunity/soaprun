#include "../EditMode.h"

void if_Palette_Initialize( HWND hWnd );
void if_Palette_SavePosition( void );
void if_Palette_SetCursor(   int x, int y );
void if_Palette_SetPosition( int x, int y );
void if_Palette_GetPosition( int *px, int *py );
void if_Palette_SetSelect( int   x1, int   y1, int   x2, int   y2 );
void if_Palette_GetSelect( int *px1, int *py1, int *px2, int *py2 );
void if_Palette_VisibleSelect( BOOL b );

void if_Palette_SetRect( void );
RECT *if_Palette_GetRect( void );
bool if_Palette_IsCloseButton( int x, int y );
bool if_Palette_IsLoadButton(  int x, int y );
bool if_Palette_IsMapParts(    int x, int y );
void if_Palette_Put( enum_EditMode mode );




void          if_Palette_SetSelectedPartsIndex( unsigned char parts_index );
unsigned char if_Palette_GetSelectedPartsIndex( void                      );
