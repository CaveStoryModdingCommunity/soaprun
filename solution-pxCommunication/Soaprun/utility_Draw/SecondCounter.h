class SecondCounter
{
	DWORD _msec_start;
	int   _keta      ;

public:

	SecondCounter( int keta );
	void  Reset();
	void  Put  ( const RECT *rc_view, int x, int y );
	int   Get  ();
};
