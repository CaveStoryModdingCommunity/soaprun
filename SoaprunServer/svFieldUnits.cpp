#include <string.h>     // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <stdio.h>      // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <stdlib.h>     // mode_t / exit() / malloc() / atoi()

#ifdef NDEBUG

#include <windows.h>

#else

#include <unistd.h>     // usleep()

#endif

#include "SharedDefines.h"
#include "RequestResponse.h"
#include "svFieldUnits.h"
#include "DebugLog.h"
#include "Signal.h"


#ifdef NDEBUG

static void _sleep_msec( int msec ){ Sleep( msec ); }

#else

static void _sleep_msec( int msec ){ usleep( msec * 1000 ); }

#endif

static void *_Thread_svFieldUnits( void *arg )
{
	bool           b_ret = false           ;
	svFieldUnits   *svu  = (svFieldUnits*)arg;

#ifdef NDEBUG
	goto End;
#else
	if( !Signal_Mask( NULL ) ) goto End;
#endif

	printf( "sv field unit on.\n" );

	while( !g_exit )
	{
		svu->ActionNPUs();
		_sleep_msec( 133 ); // 8 frame
	}

	b_ret = true;
End:
	
	return arg;
}

#ifdef NDEBUG

bool svFieldUnits::_mtx_lock  (){ return true; }
void svFieldUnits::_mtx_unlock(){  }

#else

bool svFieldUnits::_mtx_lock  (){ if( pthread_mutex_lock  ( &_mtx ) && _b_exit ) return false; return true; }
void svFieldUnits::_mtx_unlock(){     pthread_mutex_unlock( &_mtx ); }

#endif


svFieldUnits::svFieldUnits( short max_player, short max_npu, dt_MapData *map, const dt_MapData *att )
{
	int map_w =  map->get_w();
	int map_l =  map->get_l();

	_update_id =     1;
	_b_exit    = false;
	_npugrid   = new NPU_MapGrid( map, att );
	_map       = map;
	_weather   = 0;

	_players = (svFldUnit**)malloc( sizeof(svFldUnit*) * max_player );
	memset( _players, 0,            sizeof(svFldUnit*) * max_player );
	_max_player = max_player;
	for( int u = 0; u < max_player; u++ ) _players[ u ] = new svFldUnit();

	_npus    = (svFldUnit**)malloc( sizeof(svFldUnit*) * max_npu    );
	memset( _npus, 0,            sizeof(svFldUnit*) * max_npu    );
	_max_npu = max_npu;
	for( int u = 0; u < max_npu; u++ )
	{
		_npus[ u ] = new svFldUnit();
		_npus[ u ]->NPU_SetReference( _players, max_player, map, att, _npugrid, &_weather );
	}

#ifdef NDEBUG
#else
	memset( &_mtx, 0, sizeof(pthread_mutex_t) );
	pthread_mutex_init( &_mtx, NULL );
	int th_stat = pthread_create( &_thrd, NULL, _Thread_svFieldUnits, (void *)this );
	if( th_stat ){ dlog( "thrd sv field units: ERR." ); }
	else         { dlog( "thrd sv field units: OK."  ); }
#endif

}

svFieldUnits::~svFieldUnits()
{
	if( _mtx_lock() )
	{
		_b_exit = true;

//		dlog( "sv field units" );
		if( _players ){ for( int u = 0; u < _max_player; u++ ){ if( _players[ u ] ) delete _players[ u ]; } free( _players ); }
		if( _npus    ){ for( int u = 0; u < _max_npu   ; u++ ){ if( _npus   [ u ] ) delete _npus   [ u ]; } free( _npus    ); }
		if( _npugrid ) delete _npugrid;
	}

#ifdef NDEBUG
#else
	_mtx_unlock();
	pthread_mutex_destroy( &_mtx );
#endif

	dlog( "last_update_id: %d", _update_id );
}

void svFieldUnits::StartPlayer ( int player_index ){ if( _mtx_lock() ) _players[ player_index ]->PlayerStart(); _mtx_unlock(); }
void svFieldUnits::RemovePlayer( int player_index ){ if( _mtx_lock() ) _players[ player_index ]->Disable(); _mtx_unlock(); }
void svFieldUnits::RemoveNPU   ( int npu_index    ){ if( _mtx_lock() ) _npus   [ npu_index    ]->Disable(); _mtx_unlock(); }

bool svFieldUnits::SetPlayerPosition( int player_index, unsigned char gene_count, const char *p_pos_his_num )
{
	bool           b_ret   = false;
	svFldUnit      *p_unit;
	char           his_num = *p_pos_his_num;
	const POSSHORT *p_pos  = (const POSSHORT*)(p_pos_his_num + 1);

	if( !_mtx_lock()                ) goto End;
	if( !_update_id                 ) goto End;
	if( player_index >= _max_player ) goto End;

	p_unit             = _players[ player_index ];
	p_unit->gene_count = gene_count              ;
	p_unit->update_id  = _update_id              ;

	for( int h = 0; h < his_num; h++ ){ p_unit->pos_his.Push( p_pos, _update_id ); p_pos++; }

//	printf( "a[%2d] poshis: %d\n", player_index, his_num );

	_update_id++;
	b_ret = true;
End:
	_mtx_unlock();
	return b_ret;
}

bool svFieldUnits::GetPlayer( unsigned char player_index, char *p_status, char *p_param1, char *p_param2 )
{
	bool            b_ret  = false;
	const svFldUnit *p_unit;

	if( !_mtx_lock()                ) goto End;
	if( !_update_id                 ) goto End;
	if( player_index >= _max_player ) goto End;

	p_unit    = _players[ player_index ];
	*p_status = p_unit->status;
	*p_param1 = p_unit->param1;
	*p_param2 = p_unit->param2;

	b_ret = true;
End:
	_mtx_unlock();
	return b_ret;
}


bool svFieldUnits::ReadyNPU( int npu_index, char param1, char param2, short x, short y )
{
	bool      b_ret = false;

	if( npu_index >= _max_npu ) return false;

	if( !_mtx_lock()          ) goto End;
	if( !_update_id           ) goto End;

	_npus[ npu_index ]->NPU_Ready( param1, param2, x, y, _update_id );

	b_ret = true;
End:
	_mtx_unlock();
	return b_ret;
}

void svFieldUnits::NPUGrid_Update( short x, short y )
{
	if( _mtx_lock() )
	{
		_npugrid->Update( x, y );
	}
	_mtx_unlock();
}

void svFieldUnits::PushMapParts( short x, short y, unsigned char parts )
{
	if( _mtx_lock() )
	{
		for( int u = 0; u < _max_player; u++ )
		{
			if( _players[ u ]->update_id ) _players[ u ]->mparts_his.Push( x, y, parts );
		}
	}
	_mtx_unlock();
}

static void _EquipLog( unsigned char player_index, char param2, const char *detail )
{
	char eq[ 10 ]; strcpy( eq, "----" );
	if( param2 & EQUIPFLAG_SWORD  ) eq[ 3 ] = 'S';
	if( param2 & EQUIPFLAG_CROWN  ) eq[ 2 ] = 'C';
	if( param2 & EQUIPFLAG_SHIELD ) eq[ 1 ] = 'H';
	dlog( "a[%2d] lastEQ %s %s", player_index, detail, eq );
}

bool svFieldUnits::Inquiry_HITNPU     ( unsigned char player_index, unsigned char player_gene_count, unsigned char uc_tgt )
{
	bool b_ret    = false;
	bool b_update = false;


	if( uc_tgt       >= _max_npu    ) return false;
	if( player_index >= _max_player ) return false;

	svFldUnit *p_ply = _players[ player_index ];
	svFldUnit *p_npu = _npus   [ uc_tgt       ];

	if( !_mtx_lock() ) goto End;

	dlog( "a[%2d] hit [%2d] %s.", player_index, uc_tgt, g_npu_names[ p_npu->param1 ] );

	switch( p_npu->param1 )
	{
	case NPU_Goal  :

		if( p_ply->status == fuSTAT_Idle )
		{
			
			p_ply->status = fuSTAT_Goal; _EquipLog( player_index, p_ply->param2, "Goal" );
			b_update      = true       ;

			for( int u = 0; u < _max_npu; u++ )
			{
				_npus[ u ]->NPU_NotifyPlayerGoal( player_index, p_ply->gene_count, _update_id );
			}
		}
		break;

	case NPU_Closer:
	case NPU_Crawl :
	case NPU_Chase :
	case NPU_Snail :

		if( p_ply->status == fuSTAT_Idle && p_npu->status == fuSTAT_Idle )
		{
			if( p_ply->param2 & EQUIPFLAG_SWORD )
			{
				if( p_npu->Beat( player_index, _update_id ) )
				{
					b_update = true;

					if( ++p_ply->beat_count == 10 ) //4 )
					{
						dlog( "a[%2d] Crown.", player_index );
						p_ply->param2 |=  EQUIPFLAG_CROWN;
						p_ply->param2 &= ~EQUIPFLAG_SWORD;
					}
				}
			}
			else
			{
				p_npu->beat_count++;
				p_ply->status = fuSTAT_Dead; _EquipLog( player_index, p_ply->param2, "Dead" );
				b_update      = true       ;
			}
		}
		break;

	case NPU_Wuss:

		if( p_ply->status == fuSTAT_Idle && p_npu->status == fuSTAT_Idle )
		{
			if( p_npu->Beat( player_index, _update_id ) )
			{
				b_update = true;

				if( ++p_ply->beat_count == 10 ) //4 )
				{
					dlog( "a[%2d] Crown.", player_index );
					p_ply->param2 |=  EQUIPFLAG_CROWN;
					p_ply->param2 &= ~EQUIPFLAG_SWORD;
				}
			}
		}
		break;

	case NPU_Hummer :
	case NPU_Rounder:
	case NPU_Gate   :
	case NPU_Cross  :

		if( p_ply->status == fuSTAT_Idle && p_npu->status == fuSTAT_Idle )
		{
			p_ply->status = fuSTAT_Dead; _EquipLog( player_index, p_ply->param2, "Dead" );
			b_update      = true;
		}
		break;

	case NPU_Sword :

		if( p_ply->status == fuSTAT_Idle && p_npu->status == fuSTAT_Idle )
		{
			if     ( p_ply->param2 & EQUIPFLAG_SWORD ){}
			else if( p_npu->Held( player_index, player_gene_count, _update_id ) )
			{
				p_ply->param2 |= EQUIPFLAG_SWORD;
				b_update       = true;
			}
		}
		break;

	case NPU_Shield:

		if( p_ply->status == fuSTAT_Idle && p_npu->status == fuSTAT_Idle )
		{
			if     ( p_ply->param2 & EQUIPFLAG_SHIELD ){}
			else if( p_npu->Held( player_index, player_gene_count, _update_id ) )
			{
				p_ply->param2 |= EQUIPFLAG_SHIELD;
				b_update       = true;
				_weather       = WEATHER_Rain;
			}
		}
		break;
	}

	if( b_update ) _update_id++;

End:
	_mtx_unlock();
	return b_ret;

}
bool svFieldUnits::Inquiry_HitVenger  ( unsigned char player_index )
{
	bool b_ret    = false;

	if( player_index >= _max_player ) return false;

	svFldUnit *p_ply = _players[ player_index ];

	if( !_mtx_lock() ) goto End;

	p_ply->status = fuSTAT_Ghost; _EquipLog( player_index, p_ply->param2, "Ghost" );

	p_ply->param2 = 0;
	dlog( "a[%2d] Ghost.", player_index );

	_update_id++;

End:
	_mtx_unlock();
	return b_ret;
}
bool svFieldUnits::Inquiry_ChangeColor( unsigned char player_index, char param1 )
{
	bool b_ret    = false;
	bool b_update = false;

	if( player_index >= _max_player ) return false;

	svFldUnit *p_ply = _players[ player_index ];

	if( !_mtx_lock() ) goto End;

	if( p_ply->status == fuSTAT_Idle )
	{
		p_ply->param1 = param1;
		b_update      = true  ;
	}
	
	if( b_update ) _update_id++;

End:
	_mtx_unlock();
	return b_ret;
}

int svFieldUnits::GetUnits( char *buf, int buf_size, int atnd_index, unsigned int *p_last_update_id )
{
	if( atnd_index >= _max_player ) return -1;

	svFldUnit      *p_unit;
	resFIELD_fUNIT *p_sub ;
	resFIELD_HEAD  *p_head   = (resFIELD_HEAD*)buf;
	char           *p        = buf;
	int            data_size = 0;
	int            head_size = sizeof(resFIELD_HEAD );
	int            sub_size  = sizeof(resFIELD_fUNIT);
	int            pos_his_size;
	bool           b_ret     = false;

	unsigned int   old_update_id = *p_last_update_id;

	short x_start;
	short y_start;
	short x_end  ;
	short y_end  ;

	if( !_mtx_lock()         ) goto End;

	p_unit = _players[ atnd_index ];

	x_start = ( p_unit->pos_his.LastX() / ROOMGRIDNUM_H - 1 ) * ROOMGRIDNUM_H;
	y_start = ( p_unit->pos_his.LastY() / ROOMGRIDNUM_V - 1 ) * ROOMGRIDNUM_V;
	x_end   = x_start + ROOMGRIDNUM_H * 3 + 1;
	y_end   = y_start + ROOMGRIDNUM_V * 3 + 1;

	if( buf_size < head_size ) goto End;

	p_head->own_status     = p_unit->status;
	p_head->own_param1     = p_unit->param1;
	p_head->own_param2     = p_unit->param2;

	p_head->num_other      =              0;
	p_head->num_npu        =              0;
	p_head->num_mparts     =              0;
	p_head->weather        = _weather      ;

	p += head_size; data_size += head_size;

	// Other Player
	for( unsigned char u = 0; u < _max_player; u++ )
	{
		p_unit = _players[ u ];

		if( u != atnd_index && p_unit->update_id )
		{
			if( data_size + sub_size > buf_size ){ dlog( "get units: NE buf size! 1" ); goto End; }
			if( *p_last_update_id < p_unit->update_id ) *p_last_update_id = p_unit->update_id;

			p_sub = (resFIELD_fUNIT*)p;

			p_sub->pos_his_num = p_unit->pos_his.Num( old_update_id );
			p_sub->index       = u                 ;
			p_sub->gene_count  = p_unit->gene_count;
			p_sub->status      = p_unit->status    ;
			p_sub->param1      = p_unit->param1    ;
			p_sub->param2      = p_unit->param2    ;

			p += sub_size; data_size += sub_size;

			if( p_sub->pos_his_num )
			{
				pos_his_size = p_sub->pos_his_num * sizeof(POSSHORT);
				if( data_size + pos_his_size > buf_size ){ dlog( "get units: NE buf size! 2" ); goto End; }
				p_unit->pos_his.Get( (POSSHORT*)p, old_update_id );
				p += pos_his_size; data_size += pos_his_size;
			}
			p_head->num_other++;
		}
	}

	// None Play Unit
	for( unsigned char u = 0; u < _max_npu; u++ )
	{
		p_unit = _npus[ u ];

		if( p_unit->update_id &&
			p_unit->pos_his.LastX() >= x_start &&
			p_unit->pos_his.LastX() <  x_end   &&
			p_unit->pos_his.LastY() >= y_start &&
			p_unit->pos_his.LastY() <  y_end   )
		{
			if( data_size + sub_size > buf_size ){ dlog( "get objs: NE buf size! 1" ); goto End; }
			if( *p_last_update_id < p_unit->update_id ) *p_last_update_id = p_unit->update_id;

			p_sub = (resFIELD_fUNIT*)p;

			p_sub->pos_his_num = p_unit->pos_his.Num( old_update_id );
			p_sub->index       = u                 ;
			p_sub->gene_count  = p_unit->gene_count;
			p_sub->status      = p_unit->status    ;
			p_sub->param1      = p_unit->param1    ;
			p_sub->param2      = p_unit->param2    ;

			p += sub_size; data_size += sub_size;

			if( p_sub->pos_his_num )
			{
				pos_his_size = p_sub->pos_his_num * sizeof(POSSHORT);
				if( data_size + pos_his_size > buf_size ){ dlog( "get objs: NE buf size! 2" ); goto End; }
				p_unit->pos_his.Get( (POSSHORT*)p, old_update_id );
				p += pos_his_size; data_size += pos_his_size;
			}
			p_head->num_npu++;
		}
	}

	// mParts History
	p_head->num_mparts = _players[ atnd_index ]->mparts_his.Pop( (MAPPARTSCHANGE*)p );
	data_size += p_head->num_mparts * sizeof(MAPPARTSCHANGE);

//	printf( "a[%2d] send units(%d,%d)\n", atnd_index, p_head->num_other, p_head->num_npu );

	b_ret = true;

End:
	_mtx_unlock();

	if( !b_ret ) return -1;

	return data_size;
}


void svFieldUnits::ActionNPUs()
{
	if( _mtx_lock() )
	{
		for( short u = 0; u < _max_npu; u++ )
		{
			_npus[ u ]->sign = FUSIGN_None;
			_npus[ u ]->NPU_Action( _update_id );
			switch( _npus[ u ]->sign )
			{

			case FUSIGN_disCorpse:

				short npx;
				short npy;
				unsigned char parts;
				npx = _npus[ u ]->pos_his.LastX();
				npy = _npus[ u ]->pos_his.LastY();
				parts = _map->GetParts( npx, npy );

				if( ( parts >> 4 )% 2 )
				{
					parts = _map->SetCorpse( npx, npy, false );
					if( parts )
					{
						_npugrid->Update( npx, npy );
						for( int u = 0; u < _max_player; u++ )
						{
							if( _players[ u ]->update_id )
							{
								_players[ u ]->mparts_his.Push( npx, npy, parts );
							}
						}
					}
				}
				break;
			}
		}
		_update_id++;
	}
	_mtx_unlock();
}
