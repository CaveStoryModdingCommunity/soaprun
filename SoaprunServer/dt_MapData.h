#ifdef NDEBUG

#include <windows.h>

#else

#include <pthread.h>

#endif

class dt_MapData
{
	int            _max_w  ;
	int            _max_l  ;
	unsigned char* _p      ;
	int            _w      ;
	int            _l      ;

#ifdef NDEBUG
	CRITICAL_SECTION _cs;
#else
	pthread_mutex_t _mtx   ;
#endif


	bool            _b_exit;
	
	bool _mtx_lock  ();
	void _mtx_unlock();
	void _Clear( void );

public:

	 dt_MapData( int max_w, int max_l );
	~dt_MapData(                      );

	bool SetSize( int w, int l, int align_flags );

	bool Load ( const char *name );
	bool Save ( const char *name );

	int            get_w() const { return _w; }
	int            get_l() const { return _l; }
	unsigned char *get_p(){ return _p; }

	unsigned char SetCorpse( int x, int y, bool b );
	unsigned char GetParts ( int x, int y ) const;
	bool          SetMapParts( int x, int y, unsigned char mparts );

	bool GetClip( dt_MapData *p_dst, int src_x, int src_y );
	bool CopyTo ( dt_MapData *p_dst );
	void Fill   ( unsigned char uc );
};
