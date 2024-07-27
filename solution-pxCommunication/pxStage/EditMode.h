#pragma once
enum enum_EditMode
{
	enum_EditMode_Map = 0  ,
	enum_EditMode_FldObj,
//	enum_EditMode_FieldDoor,
	enum_EditMode_Attribute,
	enum_EditMode_Resident ,
};

void          EditMode_Initialize( HMENU hMenu );
void          EditMode_Release   ( void );
void          EditMode_Set       ( HMENU hMenu, enum_EditMode mode, int layer );
enum_EditMode EditMode_Get       ( void );
int           EditMode_Layer_Get ( void );
