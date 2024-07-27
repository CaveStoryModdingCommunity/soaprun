#include "../Fixture/Random.h"

#include "dt_MapData.h"
#include "dt_FldObj.h"

#define BUFFERSIZE_STAGENAME    16
//#define BUFFERSIZE_ROOMNAME     16

//#define MAX_STAGEROOM           10
#define MAX_ATTRIBUTE_HNUM      16
#define MAX_ATTRIBUTE_VNUM      16

#define ATTRIBUTE_NUM_H         16
#define ATTRIBUTE_NUM_V          1

#define MAPLAYERNUM              1
#define ATTRIBUTELAYERNUM        1

#define MAXFIELDUNIT            64

#define ERRFLAG_MAPPARTSIMAGE 0x01
#define ERRFLAG_ATTRIBUTE     0x04
#define ERRFLAG_lCASTIMAGE    0x20
#define ERRFLAG_lSYMBOLIMAGE  0x40
#define ERRFLAG_MAP           0x02
#define ERRFLAG_UNIT          0x08

#define MAX_MAP_W 256
#define MAX_MAP_L 256
#define MAX_ATT_W  16
#define MAX_ATT_L  16

#define FLDOBJ_BMP_W 128
#define FLDOBJ_BMP_L 256

//#define MAX_ENCOUNTLEVEL 4

typedef struct
{
	int mparts      ;
	int attr1       ;
	int attr2       ;
	int unit_lcast  ;
	int unit_lsymbol;

}STAGESURFACES;

class dt_Stage
{
private :

	const char   *_stage_ext       ;
	const char   *_map_fname       ;
	const char   *_unit_fname      ;
	const char   *_mparts_bmp      ;
	const char   *_attr_name       ;
	const char   *_unit_lcast_bmp  ;
	const char   *_unit_lsymbol_bmp;
				
	const char   *_dir_data    ;
				
	STAGESURFACES _surfs       ;
				
	dt_MapData   *_map_copies[ MAPLAYERNUM ];
	dt_MapData   *_map_tables[ MAPLAYERNUM ];

	dt_MapData   *_atts[ ATTRIBUTELAYERNUM ];
				
	dt_FldObj    *_unit_main;
	dt_FldObj    *_unit_copy;

	bool _Attribute_Save();
	bool _Attribute_Load();

	char _stage_name[ BUFFERSIZE_STAGENAME ];
//	char _room_name [ BUFFERSIZE_ROOMNAME  ];

	void _Map_PutAttribute( const RECT* p_rc_view, int x, int y, int parts );

public  :

	const char* Ext     (){ return _stage_ext; }

//	int  room_no  ;
	int  map_max_w;
	int  map_max_l;

	// エディターによる変更があったか
	bool b_map_changed ; 
	bool b_unit_changed;
	bool b_attr_changed;

	 dt_Stage( const char* dir_data, int max_w, int max_l, const STAGESURFACES* p_surfs );
	~dt_Stage();

	bool edit_IsChanged ();

	const char* Stage_GetName();
	bool        Stage_New ( const char* stage_name );
	bool        Stage_Save();
	int         Stage_Load( const char* stage_name/*, int room_no*/ );
	int         Stage_ReloadImages();

	// Map
	bool Map_New    ( int w, int l );
	bool Map_SaveNow();
	bool Map_Load   ();
	bool Map_SetSize( int   w, int   l, int align_flags );
	void Map_GetSize( int *pw, int *pl );
	void Map_Put          ( int layer, const RECT* p_rc_view, int off_x, int off_y, bool b_visible_zero_parts );
	void Map_Put_Attribute( int layer, const RECT* p_rc_view, int off_x, int off_y );

	void Map_copy_GetSize      ( int *pw, int *pl, int layer );
	long Map_copy_GetPartsIndex( int layer );
	void Map_copy_Copy         ( int layer, int x1, int y1, int x2, int y2, BOOL bMapParts );
	void Map_copy_Paste        ( int layer, int x1, int y1                                 );

	long Map_GetAttribute( int att_layer, int map_x, int map_y );
	long Map_GetAttribute_CheckTo( int map_x, int map_y );

	bool Map_GetClip( dt_MapData *dst, int src_x, int src_y );

	// Attribute
	void Attribute_Clear( void );
	bool Attribute_SetSize( int w, int h, int flags );
	void Attribute_GetSize    ( int *pw, int *pl );
	void Attribute_copy_Paste ( int layer, int x, int y, unsigned char parts_index );
	void Attribute_Put( int layer, RECT* p_rc_view, int off_x, int off_y );

	bool Attribute_Get( dt_MapData *dst );

	// Unit
	dt_FldObj* Unit_GetPointer     ( void );
	dt_FldObj* Unit_GetPointer_Copy( void );
	unsigned char Unit_copy_GetUnitNo ( void );

	bool Unit_copy_Copy( int x1, int y1, int x2, int y2, BOOL bMapParts );
	void Unit_Sort_UP  ( void );
	bool Unit_Delete   ( int x1, int y1, int x2, int y2 );
	int  Unit_Search   ( int x, int y );
	bool Unit_Get      ( int u, DT_FLDOBJ* p_unit );
	bool Unit_Set      ( int u, const DT_FLDOBJ* p_unit );
	bool Unit_Add      ( const DT_FLDOBJ* p_unit );
	void Unit_Shift    ( int x, int y );
	void Unit_Clear    ();
	bool Unit_SaveNow  ();
	bool Unit_Load     ();



	void edit_GetStageDirectory( char* path_dir );
};
