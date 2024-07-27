enum TOOLMODE
{
	TOOLMODE_None = 0,
	TOOLMODE_Pen,
	TOOLMODE_Erase,
	TOOLMODE_Edit,
};

void     if_Tools_Initialize( HWND hWnd );
void     if_Tools_SavePosition( void );
void     if_Tools_SetPosition( int x,   int y   );
void     if_Tools_GetPosition( int *px, int *py );
void     if_Tools_SetRect( void );
RECT*    if_Tools_GetRect( void );
TOOLMODE if_Tools_GetHitButton_Mode(      int x, int y );
int      if_Tools_GetHitButton_Visible(   int x, int y );
int      if_Tools_GetHitButton_EditLayer( int x, int y );
bool     if_Tools_IsTools(                int x, int y );
void     if_Tools_Put( void );



TOOLMODE if_Tools_Mode_Get( void          );
void     if_Tools_Mode_Set( TOOLMODE mode );
void     if_Tools_Visible_Reverse( int layer );
bool     if_Tools_Visible_Get(     int layer );
int  if_Tools_EditLayer_Get( void      );
void if_Tools_EditLayer_Set( int layer );
/*
bool if_Tools_DoorVisible_GetHitButton( int x, int y );
void if_Tools_DoorVisible_Reverse();
bool if_Tools_DoorVisible_Is();
*/
bool if_Tools_AttrVisible_GetHitButton( int x, int y );
void if_Tools_AttrVisible_Reverse();
bool if_Tools_AttrVisible_Is();
