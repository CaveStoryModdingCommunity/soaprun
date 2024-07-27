
#define POSHISTORY_NUM 8

typedef struct
{
	short x, y;
}
POSSHORT;

class PosHistory
{
private:

	char     _num;
	POSSHORT _list[ POSHISTORY_NUM ];

public :

	PosHistory();
	void Reset    ( short fld_x, short fld_y );
	void Reset    ( const POSSHORT *p_pos    );
	void PushPos  ( short fld_x, short fld_y );
	void PushPos  ( const POSSHORT *p_pos    );
	void ShiftPush( char  dir_x, char dir_y  );

	short LastX     () const;
	short LastY     () const;
	short NextX     () const;
	short NextY     () const;
	int   LastX_Real() const;
	int   LastY_Real() const;
	int   NextX_Real() const;
	int   NextY_Real() const;

	int PopOne();
	int Pop( POSSHORT *list );
};
