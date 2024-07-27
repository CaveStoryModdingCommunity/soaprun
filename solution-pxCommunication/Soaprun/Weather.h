#include "../Fixture/Random.h"

#include "Caret.h"

class Weather
{
private:

	int   _caret_num;
	int   _h, _w;

	Caret  **_carets;
	Pane   *_pane   ;
	Random *_ran    ;

	WEATHER _weather;

	unsigned char _wt_ct;

	void _Set_RainDrop();

public :

	 Weather( int w, int h, int caret_num, Random *ran );
	Pane *Pane_Get();
	~Weather();

	void Set( WEATHER wt );

	void Procedure();

	bool IsRain();
};
