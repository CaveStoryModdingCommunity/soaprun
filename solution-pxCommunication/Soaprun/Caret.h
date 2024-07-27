#include "utility_Draw/Pane.h"


enum CARETTYPE
{
	CARET_Disable = 0,
	CARET_RainDrop   ,
};

class Caret
{
private:

	Pane       *_pane    ;
	int        _x        ;
	int        _y        ;

	int        _hfW      ;
	int        _hfH      ;


	CARETTYPE  _type     ;
	short      _act_count;

	void _Action_RainDrop();

public :

	 Caret();

	~Caret();

	Pane *Pane_Get();

	void Set( CARETTYPE type, int x, int y );
	void Reset();

	void Action();

	bool IsDisable();
};
