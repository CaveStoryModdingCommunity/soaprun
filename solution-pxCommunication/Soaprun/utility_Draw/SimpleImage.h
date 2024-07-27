
enum SIMPLEIMAGEACTION
{
	SIMPLEIMAGE_STOP ,
	SIMPLEIMAGE_FLASH,
	SIMPLEIMAGE_SHOCK,
};

class SimpleImage
{
private:

	int                _surface_no;
	int                _x         ;
	int                _y         ;
	int                _off_x     ;
	int                _off_y     ;
	int                _ani_num   ;
	int                _ani_no    ;
	int                _ani_wait  ;
	SIMPLEIMAGEACTION  _action    ;

public:

	int W, H;

	 SimpleImage( void );
	~SimpleImage( void );

	bool Ready ( int w, int h, int surface_no      );
	bool Reload( const char* name, bool b_exterior, int ani_num );
	void SetPosition( int x, int y );
	void SetAction( SIMPLEIMAGEACTION action );
	void Action( void );
	void Put( const RECT* rc_view );

};
