class RingBuffer
{
private:

	int           _size  ;
	unsigned char *_p_buf;
	int           _buf_r ;
	int           _buf_w ;
	char          *_text ;
	bool          _b_overwrite;
	bool          _b_false;
	int           _w_loop ;
	int           _r_loop ;

	CRITICAL_SECTION _cs;

	unsigned char _Read();
	void _Write       ( unsigned char uc );
	void _Write_Length( int len          );
	int  _Read_Length ();

public:

	void Clear();
	 RingBuffer( int size );
	~RingBuffer();
	bool txt_Push( const char *fmt, ... );
	bool txt_Pop (       char *text     );
	bool bin_Push( const void *p, int size );
	int  bin_Pop (       void *p        );
	bool IsPop      ();
	bool IsOverwrite();

	int  GetSize ();
	int  GetUsing();
	int  GetLeft ();

	int  Get_r(){ return _buf_r; }
	int  Get_w(){ return _buf_w; }
};
