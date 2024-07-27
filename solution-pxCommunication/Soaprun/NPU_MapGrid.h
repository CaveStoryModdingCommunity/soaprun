class NPU_MapGrid
{
private:

	unsigned char *_p_buf;
	int           _buf_size;

	int           _map_w;
	int           _map_l;

public:

	 NPU_MapGrid( int map_w, int map_l );
	~NPU_MapGrid();

	void Clear  ( void   );
	void On     ( int x, int y );
	void Off    ( int x, int y );
	bool IsOn   ( int x, int y );

	int  W   (){ return _map_w   ; }
	int  L   (){ return _map_l   ; }
	int  Size(){ return _buf_size; }
};
