#include "dt_MapData.h"
#include "NPU_MapGrid.h"
#include "svPosHistory.h"
#include "mPrtsHistory.h"


#define CLOSER_LIMIT_BEAT 3

enum FUSIGN
{
	FUSIGN_None = 0,
	FUSIGN_disCorpse,
};

class svFldUnit
{
	svFldUnit        **_players ;
	short            _num_player;
	const dt_MapData *_map      ;
	const dt_MapData *_att      ;
	NPU_MapGrid      *_npugrid  ;
	unsigned char    *_p_weather;

	char             _wait      ;
	short            _start_x   ;
	short            _start_y   ;
	short            _tgt_x     ;
	short            _tgt_y     ;

	unsigned char    _held_player_index;
	unsigned char    _held_player_gene ;
	unsigned int     _held_update_id   ;

	short            _sleeping_count;
	short            _held_player_x ;
	short            _held_player_y ;

	char _TargetPlayer     ( short npx, short npy, short lange_x, short lange_y, char equip );
	bool _ShieldPlayer     ();
	void _DeadCount        ( short count, unsigned int update_id_ );
	bool _ConsiderDirection( short npx, short npy, short tgx, short tgy, bool b_rev, unsigned int update_id_ );

	void _Action_Closer  ( unsigned int update_id_ );
	void _Action_Sword   ();
	void _Action_Crawl   ( unsigned int update_id_ );
	void _Action_Wuss    ( unsigned int update_id_ );
	void _Action_Chase   ( unsigned int update_id_ );
	void _Action_Shield  ( unsigned int update_id_ );
	void _Action_Cross   ();
	void _Action_Gate    ();
	void _Action_Snail   ( unsigned int update_id_ );

public:

	unsigned int  update_id ;
	unsigned char gene_count;
	char          status    ;
	char          param1    ;
	char          param2    ;
	char		  beat_count;
	svPosHistory  pos_his   ;
	mPrtsHistory  mparts_his;

	FUSIGN        sign      ;

	svFldUnit();

	void PlayerStart  ();
	void Disable();

	void NPU_NotifyPlayerGoal( unsigned char player_index, unsigned char gene_count, unsigned int update_id_ );

	void NPU_Ready( char param1_, char param2_, short x, short y, unsigned int _update_id_ );

	void NPU_SetReference(
		svFldUnit **players      ,
		short num_player         ,
		const dt_MapData *map    ,
		const dt_MapData *att    ,
		NPU_MapGrid      *npugrid,
		unsigned char    *p_weather );

	void NPU_Action( unsigned int update_id_ );

	bool Held( unsigned char player_index, unsigned char player_gene_count, unsigned int update_id_ );
	bool Beat( unsigned char player_index,                                  unsigned int update_id_ );
};
