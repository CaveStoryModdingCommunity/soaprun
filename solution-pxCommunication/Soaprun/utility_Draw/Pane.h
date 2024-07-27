#pragma once

class Pane
{
	enum _PANETYPE
	{
//		_PANETYPE_Vanish = 0,
		_PANETYPE_None   = 0,
		_PANETYPE_View      ,
		_PANETYPE_Fill      ,
		_PANETYPE_Image     ,
		_PANETYPE_Frame     ,
	};

	enum _PANEANI
	{
		_PANEANI_None = 0 ,
		_PANEANI_Animation,
		_PANEANI_ScrollOut,
		_PANEANI_ScrollIn ,
	};

	enum
	{
		_PANEFRAME_W      =   8,
		_PANEFRAME_H      =   8,
		_PANEFRAMEIMAGE_W = 320,
		_PANEFRAMEIMAGE_H = 240,
	};

	bool      _b_visible  ;

	_PANETYPE _type      ;
	Pane      **_children;
	int       _max_child ;

	int       _x, _y, _w, _h;

	DWORD     _color     ;

	unsigned char _surf_no;
	unsigned char _sf_h   ;
	unsigned char _sf_v   ;

	struct ANI
	{
		_PANEANI type  ;
		char     x     ;
		char     y     ;
		char     count ;
		char     c_    ;
		short    wait  ;
		short    w_    ;
	}_ani;

	void     _Zero();

public:

	int param;
	
	Pane();
	Pane( int max_child );
	~Pane();
	
	void Visible( bool b );

	void SetType_View ( int x, int y, int w, int h              );
	void SetType_Fill ( int x, int y, int w, int h, DWORD color );
	void SetType_Image( int x, int y, int w, int h, int surf_no );
	void SetType_Frame( int w, int h, DWORD bk_color, int sf_h, int sf_v );
	
	void SetPos       ( int x, int y );
	void SetAnimation ( int x, int y, int count, int wait );
	void SetAnimationY( int y        );
	void SetScrollOut ( int dir_x, int dir_y );
	void SetScrollIn  ( int dir_x, int dir_y );
	void SetSurface   ( int surf_no );

	bool Child_Add   ( Pane *child );
	bool Child_Insert( Pane *child );
	bool Child_Remove( Pane *child );

	bool IsAnimation();
	void SetAnimationGap( short gap );

	int W() const;
	int H() const;
	int GetTotal();

	void Animation();

	void Put( const RECT *rc_parent, int x, int y ) const;
};
