


class ScreenCurtain
{
private:

	enum
	{
		_MAX_CURTAINRECT = 4,
	};

	enum _CURTAINACT
	{
		_CURTAINACT_NONE = 0  ,
		_CURTAINACT_IN        ,
		_CURTAINACT_OUT_SIDE  ,
		_CURTAINACT_OUT_CENTER,
		_CURTAINACT_MASK      ,
	};

	int         _count    ;
	int         _max_count;
	_CURTAINACT _action   ;
	RECT        _rcs[ _MAX_CURTAINRECT ];
	int         _rc_num   ;
	DWORD       _color    ;
	
	
public:
	
	void Mask();

	ScreenCurtain      ( DWORD color );
	
	void Set_In        ();
	void Set_Out_Side  ();
	void Set_Out_Center();
	
	void Procedure     ();
	void Put           ();
	bool IsIdle        ();
};
