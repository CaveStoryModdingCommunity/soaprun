#pragma once

#define MAPALIGNFLAG_RIGHT   0x01
#define MAPALIGNFLAG_BOTTOM  0x02
#define MAPALIGNFLAG_DISABLE 0x80

#define FIELDGRID              16

#define MAPPARTS_NUM_H 16
#define MAPPARTS_NUM_V 16
#define MAPPARTS_BMP_W (FIELDGRID * MAPPARTS_NUM_H)
#define MAPPARTS_BMP_H (FIELDGRID * MAPPARTS_NUM_V)

/*
#define DOOR_BMP_W  96 
#define DOOR_BMP_H 128
*/

enum MDLOADSTATUS
{
	MDLOAD_Disable = 0,
	MDLOAD_TryRequest ,
	MDLOAD_Requested  ,
	MDLOAD_Loaded     ,
};

typedef struct
{
	MDLOADSTATUS status;
	char         x_    ;
	char         y_    ;
}
MDLOCATION;

class dt_MapData
{
	int            _max_w      ;
	int            _max_l      ;
	unsigned char* _p          ;
	int            _w          ;
	int            _l          ;
	int            _surf_mparts;

public:

	MDLOCATION     location;

	 dt_MapData( int max_w, int max_l, int surf_mparts );
	~dt_MapData(                                       );

	void Clear( void );
	bool SetSize( int w, int l, int align_flags );

	bool Read ( FILE* fp );
	bool Write( FILE* fp );

	int get_w(){ return _w; }
	int get_l(){ return _l; }

	void          SetParts( int x, int y, unsigned char parts );
	unsigned char GetParts( int x, int y );

	void Put( const RECT* p_rc_view, int off_x, int off_y, bool b_visible_zero_parts );

	bool GetClip( dt_MapData *p_dst, int src_x, int src_y );
	bool CopyTo ( dt_MapData *p_dst );
	void Fill   ( unsigned char uc );
	void Location_Wipeout( char shift_x, char shift_y );
};
