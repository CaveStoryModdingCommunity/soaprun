#ifdef NDEBUG

#include <windows.h>

#else

#include <pthread.h>

#endif

#include "svFldUnit.h"

class svFieldUnits
{
private:

#ifdef NDEBUG

	HANDLE           _thrd     ;
	CRITICAL_SECTION _cs       ;

#else

	pthread_t       _thrd      ;
	pthread_mutex_t _mtx       ;

#endif
	bool            _b_exit    ;

	bool _mtx_lock  ();
	void _mtx_unlock();

	short           _max_player;
	short           _max_npu   ;

	svFldUnit       **_players ;
	svFldUnit       **_npus    ;

	dt_MapData      *_map      ;
	NPU_MapGrid     *_npugrid  ;

	unsigned int    _update_id ;
	unsigned char   _weather   ;

public:
	
	 svFieldUnits( short max_player, short max_npu, dt_MapData *map, const dt_MapData *att );
	~svFieldUnits();
	
	bool SetPlayerPosition  ( int           player_index, unsigned char gene_count, const char *p_pos_his_num  );
	bool GetPlayer          ( unsigned char player_index, char *p_status, char *p_param1, char *p_param2 );
	bool ReadyNPU           ( int npu_index   , char param1, char param2, short x, short y );

	bool Inquiry_HITNPU     ( unsigned char player_index, unsigned char player_gene_count, unsigned char uc_tgt );
	bool Inquiry_HitVenger  ( unsigned char player_index                                                        );
	bool Inquiry_ChangeColor( unsigned char player_index,                                  char param1          );

	void NPUGrid_Update( short x, short y );
	void PushMapParts( short x, short y, unsigned char parts );
	int  GetUnits    ( char *buf, int buf_size, int atnd_index, unsigned int *p_last_update_id );
	void StartPlayer ( int player_index );
	void RemovePlayer( int player_index );
	void RemoveNPU   ( int npu_index    );
	void ActionNPUs  ();
};
