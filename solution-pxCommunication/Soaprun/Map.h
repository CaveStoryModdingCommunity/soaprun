#pragma once

#include "../DataTable/dt_MapData.h"

#include "utility_Draw/Pane.h"

class Map
{
private:

	dt_MapData *_nine[ 3 ][ 3 ];
	dt_MapData *_att;

	Pane       *_pane ;
	Pane       **_parts;
	char       _name [ BUFSIZE_MAPNAME ];

	void _ClearParts ();

	void _Nine_Wipeout( char x, char y, char dir_x, char dir_y );

public:

	Map                ();
	~Map               ();

	void  Pane_Update();
	Pane *Pane_Get   () const;

	void          Attribute_Set ( int w, int l, const unsigned char *p );
	unsigned char Attribute_Get ( int map_x, int map_y ) const;

	bool          Room_Set      ( char location_x, char location_y, const unsigned char *p );

	bool Location_Set      ( char location_x, char location_y );
	bool Location_Shift    ( char shift_x   , char shift_y    );
	bool Location_Check    ( char dir_x     , char dir_y      );
	void Location_Get      ( char *p_lct_x  , char *p_lct_y   ) const;
	void Location_GetOffset( int  *p_off_x  , int  *p_off_y   ) const;

	void TryRequest();
	
	bool IsSide        ( short fld_x, short fld_y, char *p_dir_x, char *p_dir_y ) const;

	const char *Name() const;

	void          SetMparts( short fld_x, short fld_y, unsigned char parts );
	unsigned char GetMparts( short fld_x, short fld_y ) const;

};
