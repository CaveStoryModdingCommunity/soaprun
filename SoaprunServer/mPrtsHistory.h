
#define MAPPARTSHISTORY_NUM 16

typedef struct
{
	short         x, y;
	unsigned char parts;
}
MAPPARTSCHANGE;

class mPrtsHistory
{
private:
	int            _num                        ;
	MAPPARTSCHANGE _list[ MAPPARTSHISTORY_NUM ];
	
public:

	mPrtsHistory();

	void Push( short x, short y, unsigned char parts );

	int  Pop( MAPPARTSCHANGE *list );
	int  Num ();
	void Clear();
};
