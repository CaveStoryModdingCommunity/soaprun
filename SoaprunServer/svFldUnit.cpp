#include <stdio.h>      // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()

#include "svFldUnit.h"
#include "SharedDefines.h"
#include "DebugLog.h"


static char _player_color = 0;


svFldUnit::svFldUnit()
{
	update_id   =              0;
	status      = fuSTAT_Disable;
	param1      =              0;
	param2      =              0;
	_wait       =              0;
	gene_count  =              0;
	sign        = FUSIGN_None   ;
}

void svFldUnit::PlayerStart  ()
{
	status     = fuSTAT_Idle   ;
	param1     = _player_color ;
	param2     =              0;
	beat_count =              0;

	_player_color = (++_player_color) % 4;

//	printf( "player color %d\n", param1 );
}

void svFldUnit::Disable()
{
	status     = fuSTAT_Disable;
	update_id  =              0;
	_wait      =              0;
	pos_his.Reset( 0, 0, 0 );
}

void svFldUnit::NPU_SetReference
(
	svFldUnit        **players ,
	short            num_player,
	const dt_MapData *map      ,
	const dt_MapData *att      ,
	NPU_MapGrid      *npugrid  ,
	unsigned char    *p_weather )
{
	_players    = players   ;
	_num_player = num_player;
	_map        = map       ;
	_att        = att       ;
	_npugrid    = npugrid   ;
	_p_weather  = p_weather ;
}


void svFldUnit::NPU_Ready( char param1_, char param2_, short x, short y, unsigned int update_id_ )
{
	status     = fuSTAT_Idle;
	param1     = param1_    ;
	param2     = param2_    ;
	beat_count =           0;
	update_id  = update_id_ ;
	gene_count =           0;
	pos_his.Push( x, y, update_id );
	_tgt_x     = x;
	_tgt_y     = y;


	switch( param1_ )
	{
	case NPU_Goal   :
	case NPU_Closer :
	case NPU_Sword  :
	case NPU_Crawl  :
//	case NPU_Hummer :
//	case NPU_Rounder:
	case NPU_Wuss   :
	case NPU_Chase  :
//	case NPU_Gate   :
	case NPU_Shield :
//	case NPU_Cross  :
	case NPU_Snail  :
		_npugrid->On( x, y );
		break;
	}

	_start_x   = x;
	_start_y   = y;
}

typedef struct
{
	char x;
	char y;
}
_DIRCHAR;

enum  _DIRECTION                             { _DIR_LEFT = 0, _DIR_UP   , _DIR_RIGHT, _DIR_DOWN , _DIR_num  , };
static const _DIRCHAR _dir_tbl[ _DIR_num ] = { { -1,  0 },    {  0, -1 }, {  1,  0 }, {  0,  1 }              };

static short _Absolute_short( short a )
{
	if( a < 0 ) return -a;
	return a;
}

void svFldUnit::_DeadCount( short count, unsigned int update_id_ )
{
	short      npx, npy;
	if( !pos_his.GetLast( &npx, &npy ) ) return;
	if( ++_sleeping_count > count )
	{
		gene_count++;
		_wait = 24;

		_npugrid->Off( npx     , npy      );
		_npugrid->On ( _start_x, _start_y );
		npx = _start_x;
		npy = _start_y;
		pos_his.Reset( _start_x, _start_y, update_id_ );
		status = fuSTAT_Suspend;
	}
}

char svFldUnit::_TargetPlayer( short npx, short npy, short lange_x, short lange_y, char equip )
{
	short           min_abs = 0x7fff;
	short           abs;
	char            tgt_pl = -1;
	const svFldUnit *p_pl;
	short           plx, ply;

	char mask = ~equip;
	char mmmm  = -1;

	// Search Target.
	for( int pl = 0; pl < _num_player; pl++ )
	{
		p_pl = _players[ pl ];

		if( p_pl->status == fuSTAT_Idle                  &&
			p_pl->pos_his.GetLast( &plx, &ply )          &&
			plx >= npx - lange_x && plx <= npx + lange_x &&
			ply >= npy - lange_y && ply <= npy + lange_y &&
			( p_pl->param2 | mask ) == mmmm )
		{
			abs = _Absolute_short( plx - npx ) + _Absolute_short( ply - npy );

			if( min_abs >= abs ){ min_abs = abs; tgt_pl = pl; }
		}
	}
	return tgt_pl;
}

bool svFldUnit::_ShieldPlayer()
{
	const svFldUnit *p_pl;
	for( int pl = 0; pl < _num_player; pl++ )
	{
		p_pl = _players[ pl ];
		if( p_pl->status == fuSTAT_Idle && p_pl->param2 & EQUIPFLAG_SHIELD ) return true;
	}
	return false;
}

#define _DIRROOT_NUM 8
#define _DIR_NUM     3

bool svFldUnit::_ConsiderDirection( short npx, short npy, short tgx, short tgy, bool b_rev, unsigned int update_id_ )
{
	_DIRECTION dir      = _DIR_num;
	_DIRECTION last_dir = _DIR_num;

	_DIRECTION pri1, pri2;

	if( _Absolute_short( tgx - npx ) > _Absolute_short( tgy - npy ) )
	{
		if     ( tgx < npx ) dir  = _DIR_LEFT ;
		else if( tgx > npx ) dir  = _DIR_RIGHT;

		if     ( tgy < npy ){ pri1 = _DIR_UP   ; pri2 = _DIR_DOWN ; }
		else                { pri1 = _DIR_DOWN ; pri2 = _DIR_UP   ; }
	}
	else
	{
		if     ( tgy < npy ) dir  = _DIR_UP   ;
		else if( tgy > npy ) dir  = _DIR_DOWN ;

		if     ( tgx < npx ){ pri1 = _DIR_LEFT ; pri2 = _DIR_RIGHT; }
		else                { pri1 = _DIR_RIGHT; pri2 = _DIR_LEFT ; }
	}

	_DIRECTION dir_tbl[ _DIRROOT_NUM ][ _DIR_NUM ]; 

	for( int t = 0; t < _DIRROOT_NUM; t++ )
	{
		for( int d = 0; d < _DIR_NUM; d++  ) dir_tbl[ t ][ d ] = _DIR_num;
	}

	switch( dir )
	{
	case _DIR_DOWN:
	case _DIR_UP  :

		if( b_rev )
		{
			if( dir == _DIR_DOWN ) dir = _DIR_UP  ;
			else                   dir = _DIR_DOWN;
		}
		dir_tbl[ 0 ][ 0 ]                                                     = dir ;
		dir_tbl[ 1 ][ 0 ] = pri1; dir_tbl[ 1 ][ 1 ]                           = dir ;
		dir_tbl[ 2 ][ 0 ] = pri2; dir_tbl[ 2 ][ 1 ]                           = dir ;
		dir_tbl[ 3 ][ 0 ] = pri1; dir_tbl[ 3 ][ 1 ] = pri1; dir_tbl[ 3 ][ 2 ] = dir ;
		dir_tbl[ 4 ][ 0 ] = pri2; dir_tbl[ 4 ][ 1 ] = pri2; dir_tbl[ 4 ][ 2 ] = dir ;
		dir_tbl[ 5 ][ 0 ] = pri1;
		dir_tbl[ 6 ][ 0 ] = pri2;
		if( dir == _DIR_DOWN ) dir_tbl[ 7 ][ 0 ] = _DIR_UP  ;
		else                   dir_tbl[ 7 ][ 0 ] = _DIR_DOWN;
		break;

	case _DIR_LEFT :
	case _DIR_RIGHT:

		if( b_rev )
		{
			if( dir == _DIR_LEFT ) dir = _DIR_RIGHT;
			else                   dir = _DIR_LEFT ;
		}
		dir_tbl[ 0 ][ 0 ]                                                     = dir ;
		dir_tbl[ 1 ][ 0 ] = pri1; dir_tbl[ 1 ][ 1 ]                           = dir ;
		dir_tbl[ 2 ][ 0 ] = pri2; dir_tbl[ 2 ][ 1 ]                           = dir ;
		dir_tbl[ 3 ][ 0 ] = pri1; dir_tbl[ 3 ][ 1 ] = pri1; dir_tbl[ 3 ][ 2 ] = dir ;
		dir_tbl[ 4 ][ 0 ] = pri2; dir_tbl[ 4 ][ 1 ] = pri2; dir_tbl[ 4 ][ 2 ] = dir ;
		dir_tbl[ 5 ][ 0 ] = pri1;
		dir_tbl[ 6 ][ 0 ] = pri2;
		if( dir == _DIR_RIGHT ) dir_tbl[ 7 ][ 0 ] = _DIR_LEFT ;
		else                    dir_tbl[ 7 ][ 0 ] = _DIR_RIGHT;
		break;

	default : return false;
	}

	short x, y;
	short min_abs = 0x7fff;
	short max_abs =      0;

	for( int t = 0; t < _DIRROOT_NUM; t++ )
	{
		x = npx;
		y = npy;
		for( int d = 0; d < _DIR_NUM; d++  )
		{
			if( dir_tbl[ t ][ d ] == _DIR_num )
			{
				short abs = _Absolute_short( tgx - x ) + _Absolute_short( tgy - y );
				if( b_rev ){ if( max_abs < abs  ){ max_abs = abs; last_dir = dir_tbl[ t ][ 0 ]; } }
				else       { if( min_abs > abs  ){ min_abs = abs; last_dir = dir_tbl[ t ][ 0 ]; } }
				break;
			}

			x += _dir_tbl[ dir_tbl[ t ][ d ] ].x;
			y += _dir_tbl[ dir_tbl[ t ][ d ] ].y;
			if(	_npugrid->IsOn( x, y ) ) break;

			if( d == _DIR_NUM - 1             )
			{
				short abs = _Absolute_short( tgx - x ) + _Absolute_short( tgy - y );
				if( b_rev ){ if( max_abs < abs  ){ max_abs = abs; last_dir = dir_tbl[ t ][ 0 ]; } }
				else       { if( min_abs > abs  ){ min_abs = abs; last_dir = dir_tbl[ t ][ 0 ]; } }
				break;
			}
		}
	}

	if( last_dir == _DIR_num ) return false;

	_npugrid->Off( npx, npy );
	_npugrid->On ( npx + _dir_tbl[ last_dir ].x, npy + _dir_tbl[ last_dir ].y );
	pos_his.ShiftPush(   _dir_tbl[ last_dir ].x,       _dir_tbl[ last_dir ].y, update_id_ );

	return true;
}

void svFldUnit::_Action_Closer( unsigned int update_id_ )
{
	char            tgt_pl  = -1;
	short           tgx, tgy;
	short           npx, npy;
	bool            b_rev = false;
	const svFldUnit *p_pl;

	if( !pos_his.GetLast( &npx, &npy ) ) return;

	if( beat_count >= 3 )
	{
		param1 = NPU_Wuss;
		dlog( "Closer -> Wuss." );
	}

	switch( status )
	{
	case fuSTAT_Suspend:

		if( _wait ){ _wait--; break; }
		if( _TargetPlayer( npx, npy, 4, 3, 0 ) == -1 ) break;
		status = fuSTAT_Idle;

	case fuSTAT_Idle   :

		if( _wait ){ _wait--; break; }

		tgt_pl = _TargetPlayer( npx, npy, 5, 4, 0 );
		if( tgt_pl == -1 )
		{
			tgx = _start_x; tgy = _start_y;
			if( npx == tgx && npy == tgy ){ status = fuSTAT_Suspend; break; }
		}
		else
		{
			p_pl = _players[ tgt_pl ];
			p_pl->pos_his.GetLast( &tgx, &tgy );

			if( p_pl->param2 & (EQUIPFLAG_SWORD|EQUIPFLAG_CROWN) ) b_rev = true;
		}

		if( _ConsiderDirection( npx, npy, tgx, tgy, b_rev, update_id_ ) ) _wait = 2;
		break;

	case fuSTAT_Dead: _DeadCount( 35, update_id_ ); break;
	}
}

void svFldUnit::_Action_Wuss( unsigned int update_id_ )
{
	char       tgt_pl  =     -1;
	short      tgx, tgy;
	short      npx, npy;
	bool       b_rev = false;

	const svFldUnit *p_pl;

	if( !pos_his.GetLast( &npx, &npy ) ) return;

	switch( status )
	{
	case fuSTAT_Suspend:

		if( _wait ){ _wait--; break; }
		if( _TargetPlayer( npx, npy, 4, 3, 0 ) == -1 ) break;
		status = fuSTAT_Idle;

	case fuSTAT_Idle:

		if( _wait ){ _wait--; break; }

		tgt_pl = _TargetPlayer( npx, npy, 5, 4, 0 );
		if( tgt_pl == -1 )
		{
			tgx = _start_x; tgy = _start_y;
			if( npx == tgx && npy == tgy ){ status = fuSTAT_Suspend; break; }
		}
		else
		{
			p_pl = _players[ tgt_pl ];
			p_pl->pos_his.GetLast( &tgx, &tgy );
			b_rev = true;
		}

		if( _ConsiderDirection( npx, npy, tgx, tgy, b_rev, update_id_ ) ) _wait = 2;
		break;

	case fuSTAT_Dead: _DeadCount( 35, update_id_ ); break;
	}
}

void svFldUnit::_Action_Chase( unsigned int update_id_ )
{
	char  tgt_pl = -1;
	short tgx, tgy;
	short npx, npy;
	bool  b_rev = false;
	const svFldUnit *p_pl;

	if( !pos_his.GetLast( &npx, &npy ) ) return;

	bool b_shield = false;

	switch( status )
	{
	case fuSTAT_Suspend:

		if( _wait ){ _wait--; break; }
		if( !_ShieldPlayer() ){ _wait = 2; break; }
		tgt_pl =                    _TargetPlayer( npx, npy, 5, 4, EQUIPFLAG_SHIELD );
		if( tgt_pl == -1 ) tgt_pl = _TargetPlayer( npx, npy, 5, 4,                0 );
		status = fuSTAT_Idle;

	case fuSTAT_Idle   :

		if( !_ShieldPlayer() && npx != _start_x && npy != _start_x )
		{
			status = fuSTAT_Suspend;
			break;
		}

		if( _wait ){ _wait--; break; }

		tgt_pl =                    _TargetPlayer( npx, npy, 21, 16, EQUIPFLAG_SHIELD );
		if( tgt_pl == -1 ) tgt_pl = _TargetPlayer( npx, npy,  5,  4,                0 );
		if( tgt_pl == -1 ){ status = fuSTAT_Suspend; break; }
		p_pl = _players[ tgt_pl ];
		p_pl->pos_his.GetLast( &tgx, &tgy );
//		if( p_pl->param2 & (EQUIPFLAG_CROWN) ) b_rev = true;

		if( _ConsiderDirection( npx, npy, tgx, tgy, b_rev, update_id_ ) ) _wait = 0;
		break;

	case fuSTAT_Dead: _DeadCount( 35, update_id_ ); break;
	}
}

void svFldUnit::_Action_Crawl ( unsigned int update_id_ )
{
	short           npx, npy;
	short           tgx, tgy;
	const svFldUnit *p_pl   ;
	char            tgt_pl = -1;

	if( !pos_his.GetLast( &npx, &npy ) ) return;

	switch( status )
	{
	case fuSTAT_Suspend:

		if( _wait ){ _wait--; return; }
		status = fuSTAT_Idle;

	case fuSTAT_Idle:

		if( _wait ){ _wait--; return; }

		if( npx == _start_x && npy == _start_y ) tgt_pl = _TargetPlayer( npx, npy, 1, 1, 0 );
		if( tgt_pl == -1 ){ tgx = _start_x; tgy = _start_y; }
		else              { p_pl = _players[ tgt_pl ]; p_pl->pos_his.GetLast( &tgx, &tgy ); }

		if( _ConsiderDirection( npx, npy, tgx, tgy, false, update_id_ ) ) _wait = 5;
		break;
	
	case fuSTAT_Dead: _DeadCount( 80, update_id_ ); break;
	}
}

void svFldUnit::_Action_Snail ( unsigned int update_id_ )
{
	short           npx, npy;
	short           tgx, tgy;
	const svFldUnit *p_pl   ;
	char            tgt_pl = -1;

	unsigned char parts1;

	if( !pos_his.GetLast( &npx, &npy ) ) return;

	switch( status )
	{
	case fuSTAT_Suspend:

		if( _wait ){ _wait--; return; }
		if( _TargetPlayer( npx, npy, 1, 1, 0 ) == -1 ) break;
		status = fuSTAT_Idle;

	case fuSTAT_Idle:

		if( _wait ){ _wait--; return; }

		parts1 = _map->GetParts( npx, npy );

		// Eat Corps.
		if( (parts1/0x10)%2 ){ sign  = FUSIGN_disCorpse; _wait = 12; break; }

		int d;
		for( d = 0; d < _DIR_num; d++ )
		{
			tgx = npx + _dir_tbl[ d ].x;
			tgy = npy + _dir_tbl[ d ].y;
			parts1 = _map->GetParts( tgx, tgy );
			if( (parts1/0x10)%2 && !_npugrid->IsOn( tgx, tgy ) ) break;
		}
		if( d == _DIR_num )
		{
			tgt_pl = _TargetPlayer( npx, npy, 2, 2, 0 );
			if( tgt_pl != -1 ){ p_pl = _players[ tgt_pl ]; p_pl->pos_his.GetLast( &tgx, &tgy ); }
			else              { status = fuSTAT_Suspend;; break; }
		}
		if( _ConsiderDirection( npx, npy, tgx, tgy, false, update_id_ ) ) _wait = 8;
		break;
	
	case fuSTAT_Dead: _DeadCount( 80, update_id_ ); break;
	}
}

void svFldUnit::_Action_Sword()
{
	if( status != fuSTAT_Dead ) return;

	const svFldUnit *p_pl = _players[ _held_player_index ];
	short npx  , npy  ;

	bool b_held_off = false;
	int  reason     =     0;

	if     ( p_pl->gene_count != _held_player_gene ){ b_held_off = true; reason = 1; }
	else if( p_pl->status     == fuSTAT_Dead   ||
			 p_pl->status     == fuSTAT_Ghost  ||
			 p_pl->status     == fuSTAT_Disable    ){ b_held_off = true; reason = 2; }
	else if( p_pl->update_id > _held_update_id &&
		   !(p_pl->param2 & EQUIPFLAG_SWORD)       ){ b_held_off = true; reason = 3; }

	if( b_held_off )
	{
		dlog( "a[%2d] hold off (%d)", _held_player_index, reason );
		gene_count++;
		npx    = _start_x   ;
		npy    = _start_y   ;
		status = fuSTAT_Idle;
	}
}

void svFldUnit::_Action_Shield( unsigned int update_id_ )
{
	const svFldUnit *p_pl;
	int  reason     =     0;
	bool b_held_off = false;


	switch( status )
	{
	case fuSTAT_Dead: // held.

		p_pl = _players[ _held_player_index ];

		if     ( p_pl->gene_count != _held_player_gene ){ b_held_off = true; reason = 1; }
		else if( p_pl->status     == fuSTAT_Dead   ||
				 p_pl->status     == fuSTAT_Ghost  ||
				 p_pl->status     == fuSTAT_Disable    ){ b_held_off = true; reason = 2; }
		else if( p_pl->update_id > _held_update_id &&
			   !(p_pl->param2 & EQUIPFLAG_SHIELD)      ){ b_held_off = true; reason = 3; }

		if( b_held_off )
		{
			dlog( "a[%2d] shield off (%d)", _held_player_index, reason );
			gene_count++;
			status = fuSTAT_Idle;
			_wait  = 100;
			pos_his.Reset( _tgt_x, _tgt_y, update_id_ );
			if( !_ShieldPlayer() ) *_p_weather = WEATHER_None;
		}
		else
		{
			_tgt_x = p_pl->pos_his.LastX();
			_tgt_y = p_pl->pos_his.LastY();
		}
		break;

	case fuSTAT_Idle   :

		if( pos_his.LastX() != _start_x || pos_his.LastY() != _start_y )
		{
			if( _wait ){ _wait--; break; }
			pos_his.Reset( _start_x, _start_y, update_id_ );
			_tgt_x = _start_x      ;
			_tgt_y = _start_y      ;
		}
		break;
	}

}

void svFldUnit::_Action_Cross()
{
	if( _wait ){ _wait--; return; }
	param2++;
	if( param2 > 3 ) param2 = 0;
	_wait = 32;

}

void svFldUnit::_Action_Gate()
{
	bool b1 = false;
	bool b2 = false;
	bool b3 = false;
	const svFldUnit *p_pl;

	short npx = pos_his.LastX();
	short npy = pos_his.LastY();
	short plx, ply;

	if( _wait ){ _wait--; return; }

	// Search Target.
	for( int pl = 0; pl < _num_player; pl++ )
	{
		p_pl = _players[ pl ];

		if( p_pl->status == fuSTAT_Idle && p_pl->pos_his.GetLast( &plx, &ply ) )
		{
			if( plx == npx - 3 && ply == npy + 3 ) b1 = true;
			if( plx == npx     && ply == npy + 6 ) b2 = true;
			if( plx == npx + 3 && ply == npy + 3 ) b3 = true;
		}
	}

	if( b1 && b2 && b3 )
	{
		param2 = 0;
	}
	else param2 = 1;

	_wait = 6;
}

void svFldUnit::NPU_Action( unsigned int update_id_ )
{
	sign = FUSIGN_None;
	if( !update_id  ) return;

	switch( param1 )
	{
	case NPU_Closer: _Action_Closer( update_id_ ); break;
	case NPU_Sword : _Action_Sword (            ); break;
	case NPU_Crawl : _Action_Crawl ( update_id_ ); break;
	case NPU_Wuss  : _Action_Wuss  ( update_id_ ); break;
	case NPU_Chase : _Action_Chase ( update_id_ ); break;
	case NPU_Shield: _Action_Shield( update_id_ ); break;
	case NPU_Cross : _Action_Cross (            ); break;
	case NPU_Gate  : _Action_Gate  (            ); break;
	case NPU_Snail : _Action_Snail ( update_id_ ); break;
	}
		
	update_id = update_id_;
}

bool svFldUnit::Held( unsigned char player_index, unsigned char player_gene_count, unsigned int update_id_ )
{
	if( status != fuSTAT_Idle )
	{
		dlog( "a[%2d] hold failed.", player_index );
		return false;
	}
	dlog( "a[%2d] hold ok.", player_index );

	status             = fuSTAT_Dead      ;
	_held_player_index = player_index     ;
	_held_player_gene  = player_gene_count;
	_held_update_id    = update_id_       ;
	update_id          = update_id_       ;

	return true;
}

bool svFldUnit::Beat( unsigned char player_index, unsigned int update_id_ )
{
	if( status != fuSTAT_Idle )
	{
		printf( "beat player[%d] failed.\n", player_index );
		return false;
	}
	printf( "beat player[%d] ok.\n", player_index );
	status             = fuSTAT_Dead ;
	_held_player_index = player_index;
	update_id          = update_id_  ;
	_held_update_id    = update_id_  ;
	_sleeping_count    =            0;
	beat_count         =            0;
	if( param1 == NPU_Wuss ) param1 = NPU_Closer;

	short npx, npy;
	if( pos_his.GetLast( &npx, &npy ) ) _npugrid->Off( npx, npy );

	return true;
}

void svFldUnit::NPU_NotifyPlayerGoal( unsigned char player_index, unsigned char gene_count, unsigned int update_id_ )
{
	switch( param1 )
	{
	case NPU_Shield:
		if( status != fuSTAT_Dead              ) break;
		if( _held_player_gene != gene_count    ) break;
		if( _held_player_index != player_index ) break;

		gene_count++;
		status = fuSTAT_Idle;
		_wait  = 0;
		update_id  = update_id_ ;
		_tgt_x     = _start_x;
		_tgt_y     = _start_x;
		pos_his.Reset( _start_x, _start_y, update_id_ );
		if( !_ShieldPlayer() ) *_p_weather = WEATHER_None;
		break;
	}
}
