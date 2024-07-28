
#define POSHISTORY_NUM 8

typedef struct
{
	short x, y;
}
POSSHORT;

typedef struct
{
	unsigned int update_id;
	short        x, y;
}
POSUPDATE;

class svPosHistory
{
private:

	char      _num;
	POSUPDATE _list[ POSHISTORY_NUM ];

public :

	svPosHistory();
	void Push     ( short map_x, short map_y, unsigned int update_id );
	void Push     ( const POSSHORT *p_pos   , unsigned int update_id );
	int  Get( POSSHORT *list, unsigned int update_id );
	int  Num( unsigned int update_id );
	void ShiftPush( char  dir_x, char dir_y, unsigned int update_id );
	void Reset( short x, short y, unsigned int update_id );

	bool GetLast( short *px, short *py ) const;
	short LastX() const;
	short LastY() const;
};
