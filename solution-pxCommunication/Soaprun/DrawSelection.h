
#include "utility_Input/Buttons.h"
#include "utility_Draw/Pane.h"

class DrawSelection
{
	enum
	{
		_CHIPS_X = 16,
		_CHIPS_Y =  8,
	};

	int _sel;

	Pane *_pane_frame ;
	Pane *_pane_cursor;
	Pane *_pane_chips[ DRAWCOLOR_NUM ];

	void _UpdateCursor();

public:

	DrawSelection();

	~DrawSelection();

	void  Open     ();
	void  Close    ();
	int   GetSelect();
	Pane* GetPane  ();
	void  Action( const BUTTONSSTRUCT* p_btns );
};

