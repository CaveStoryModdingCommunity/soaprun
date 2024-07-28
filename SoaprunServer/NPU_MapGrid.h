class NPU_MapGrid
{
private:

	unsigned char    *_p_buf  ;
	int              _buf_size;

	const dt_MapData *_map    ;
	const dt_MapData *_att    ;


public:

	 NPU_MapGrid( const dt_MapData *map, const dt_MapData *att );
	~NPU_MapGrid();

	void Clear  ( void   );
	
//	void SetOn  ( int x, int y );
//	void Off    ( int x, int y );
	void On     ( int x, int y );
	void Off    ( int x, int y );
	bool IsOn   ( int x, int y );

	void Update ( int x, int y );

//	int  W   (){ return _map_w   ; }
//	int  L   (){ return _map_l   ; }
//	int  Size(){ return _buf_size; }
};
