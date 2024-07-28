#ifdef NDEBUG

#include <windows.h>

#else

#include <pthread.h>

#endif

typedef struct
{
	char  host_address[ BUFFERSIZE_IPADDRESS ];
	char  host_id     [ BUFFERSIZE_HOSTID    ];
	int   sock        ;
	bool  b_use       ;
}
LINKUNIT;

class LinkUnits
{
private:
	
	LINKUNIT        *_units   ;
	int             _max_unit ;

#ifdef NDEBUG
	CRITICAL_SECTION _cs      ;
#else
	pthread_mutex_t _mtx      ;
#endif

	bool            _b_exit   ;

	int             _max_login_counter;
	
	bool _mtx_lock  ();
	void _mtx_unlock();
	
public:
	
	 LinkUnits( int max_unit );
	~LinkUnits();
	bool Push( const char *ip, const char *id, int sock );
	const LINKUNIT *GetLink( int index );
	bool RemoveBeforeClose ( int index, const LINKUNIT *p_link );
	void ShutdownAndClose  ( int index, const LINKUNIT *p_link );
};

// LinkUnits を作成。デストラクタでlockを使うのはこれが初めて…
