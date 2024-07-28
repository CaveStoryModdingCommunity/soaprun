#define UNITRECORD_FLAG_VALID 0x01

typedef struct
{
	unsigned char flags    ;
	unsigned char param1   ;
	unsigned char param2   ;

	short         x        ;
	short         y        ;
}
DT_FLDOBJ;

class dt_FldObj
{
	short      _max_unit;
	short      xxx      ;
	DT_FLDOBJ* _units   ;

public:

	 dt_FldObj( int max_unit );
	~dt_FldObj(              );

	void  Clear();
	void  Shift( short x, short y );
	int   Count();

	bool  Load ( const char *name );

	bool  Add( const DT_FLDOBJ *p_obj );
	bool  Set( const DT_FLDOBJ *p_obj, int index );
	const DT_FLDOBJ* Get(             int index );
	bool  Delete( int index );
	void  Sort_Up( void );
};
