#include <StdAfx.h>

#include "../Dx/DxDraw.h"
#include "../Fixture/Random.h"
#include "../Fixture/RingBuffer.h"
#include "../Fixture/DebugLog.h"

#include "utility_Sound/SEFF.h"
#include "utility_Sound/BGM.h"

#include "SharedDefines.h"
#include "FldUnit.h"
#include "RequestResponse.h"
#include "Triangle.h"
#include "GameRecord.h"

extern Triangle   *g_tri     ;
extern RingBuffer *g_ring_msg;
extern GameRecord *g_grec    ;

static Random _ran( 0x3b1d, 0xff19 );

FldUnit::FldUnit( const Map *map, bool b_equip )
{
	_map         = map           ;
	_pane_body   = new Pane( b_equip ? 3 : 0 ) ; 

	if( b_equip )
	{
		_pane_sword  = new Pane();
		_pane_crown  = new Pane();
		_pane_shield = new Pane();
		_pane_body  ->Child_Add( _pane_sword  ); _pane_sword ->SetAnimation( 0, 0, 0, 0 );
		_pane_body  ->Child_Add( _pane_crown  ); _pane_crown ->SetAnimation( 1, 0, 2, 8 );
		_pane_body  ->Child_Add( _pane_shield ); _pane_shield->SetAnimation( 3, 0, 0, 0 );
		_pane_sword ->SetType_Image( 0, 0, 8, 16, SfNo_Equip );
		_pane_crown ->SetType_Image( 4, 0, 8,  8, SfNo_Equip );
		_pane_shield->SetType_Image( 8, 8, 8,  8, SfNo_Equip );
	}
	else
	{
		_pane_sword  = NULL;
		_pane_crown  = NULL;
		_pane_shield = NULL;
	}

	_pane_shadow = new Pane()    ;
	_pane_shadow->SetType_Image( 0, 12, MAPPARTS_W, 4, SfNo_Shadow );
	_pane_shadow->SetAnimation ( 0,  0, 2, 0 );


	_status      = fuSTAT_Disable;
	_beat        = 0;
}

FldUnit::~FldUnit()
{
	if( _pane_body   ) delete _pane_body  ;
	if( _pane_sword  ) delete _pane_sword ;
	if( _pane_crown  ) delete _pane_crown ;
	if( _pane_shield ) delete _pane_shield;
	if( _pane_shadow ) delete _pane_shadow;
}

void FldUnit::Disable()
{
	_status = fuSTAT_Disable;
	_pane_body->Visible( false );
	if( _pane_sword  ) _pane_sword ->Visible( false );
	if( _pane_crown  ) _pane_crown ->Visible( false );
	if( _pane_shield ) _pane_shield->Visible( false );
	_pane_shadow->Visible( false );
}

bool FldUnit::Status_Set( unsigned char gene_count, char status )
{
	if( gene_count == _gene_count && _status == fuSTAT_Dead ) return false;

	int ani_wait = 16;

	if( _status != status )
	{
		switch( status )
		{
		case fuSTAT_Suspend: ani_wait = 64; break;

		case fuSTAT_Idle   :

			if     ( _ctrl == fuCTRL_Venger ) ani_wait = 0;
			else if( _ctrl == fuCTRL_NPU    )
			{
				if( _param1 == NPU_Hummer  ||
					_param1 == NPU_Rounder ||
					_param1 == NPU_Chase   ||
					_param1 == NPU_Cross      ) ani_wait =  4;
				else if( _param1 == NPU_Snail ) ani_wait = 32;
			}
			break;

		case fuSTAT_Dead   :

			SEFF_Voice_Play( _seno_scream, pos_his.LastX(), pos_his.LastY() );
			ani_wait = 8;
			break;

		case fuSTAT_Goal   :

			SEFF_Voice_Play( SeNo_Goal   , pos_his.LastX(), pos_his.LastY() );
			break;

		case fuSTAT_Ghost  :

			ani_wait = 0; _pane_shadow->Visible( false );
			break;

		default: return false;
		}
		_status = status;
		_pane_body->SetAnimation( status * 2, _param1, 2, ani_wait );
	}
	else
	{
		_pane_body->SetAnimationY( _param1 );
	}

	_gene_count = gene_count;
	
	return true;
}

bool FldUnit::Status_Set( char status )
{
	return Status_Set( _gene_count, status );
}

bool FldUnit::GetScroll()
{
	char lct_x, lct_y;

	_map->Location_Get( &lct_x, &lct_y );

	if( _x == pos_his.LastX_Real() && _y == pos_his.LastY_Real() &&
		_map->IsSide(
		pos_his.LastX() - lct_x * ROOMGRIDNUM_H,
		pos_his.LastY() - lct_y * ROOMGRIDNUM_V, NULL, NULL ) ) return true;

	return false;
}

bool FldUnit::IsEnable () const{ if( _status != fuSTAT_Disable  ) return true; return false; }
bool FldUnit::IsAlive  () const{ if( _status == fuSTAT_Idle     ) return true; return false; }
bool FldUnit::IsIdle   () const{ if( _status == fuSTAT_Idle     ) return true; return false; }
bool FldUnit::IsGoal   () const{ if( _status == fuSTAT_Goal     ) return true; return false; }
bool FldUnit::IsDead   () const{ if( _status == fuSTAT_Dead     ) return true; return false; }
bool FldUnit::IsGhost  () const{ if( _status == fuSTAT_Ghost    ) return true; return false; }
															  
bool FldUnit::IsSword  () const{ if( _param2 & EQUIPFLAG_SWORD  ) return true; return false; }

Pane *FldUnit::Pane_body()
{
	return _pane_body;
}

Pane *FldUnit::Pane_shadow()
{
	return _pane_shadow;
}

void FldUnit::Pane_Update()
{
	int x, y;

	_map->Location_GetOffset( &x, &y );

	x = _x / VS - MAPPARTS_hfW - x;
	y = _y / VS - MAPPARTS_hfH - y;

	if( _ctrl == fuCTRL_Own || _ctrl == fuCTRL_Other )
	{
		bool b_sword  = false; if( _status == fuSTAT_Idle && _param2 & EQUIPFLAG_SWORD  ) b_sword  = true; if( _pane_sword  ) _pane_sword ->Visible( b_sword  );
		bool b_crown  = false; if( _status == fuSTAT_Idle && _param2 & EQUIPFLAG_CROWN  ) b_crown  = true; if( _pane_crown  ) _pane_crown ->Visible( b_crown  );
		bool b_shield = false; if( _status == fuSTAT_Idle && _param2 & EQUIPFLAG_SHIELD ) b_shield = true; if( _pane_shield ) _pane_shield->Visible( b_shield );
	}

	_pane_body  ->SetPos( x, y                  - 2 );
	_pane_shadow->SetPos( x, y + MAPPARTS_H - 4 - 2 );
}

void FldUnit::SetUnit_Owner
(
	short  fld_x , short fld_y,
	char   dir_x , char  dir_y )
{
	_ctrl        = fuCTRL_Own;
	_param1      =          0;
	_param2      =          0;
	_x           = fld_x * MAPPARTS_W * VS;
	_y           = fld_y * MAPPARTS_H * VS;
	_dir_x       = dir_x     ;
	_dir_y       = dir_y     ;
	_beat        =          0;
	_speed       =          2;
	_seno_scream = SeNo_Scream1;

	pos_his.PushPos( fld_x, fld_y );
	SEFF_SetHear   ( fld_x, fld_y );

	_beep_interval_count = 0;

	_pane_body  ->SetType_Image( 0,  0, MAPPARTS_W, MAPPARTS_H, SfNo_fuPlayer );

	Status_Set( fuSTAT_Idle );
	_pane_shadow->Visible( true );
}

void FldUnit::_Venger_ResetPosition( int a )
{
	short fld_x = _tgt_unit->pos_his.LastX();
	short fld_y = _tgt_unit->pos_his.LastY();
	char  tgt_dir_x, tgt_dir_y;

	_tgt_unit->GetDirection( &tgt_dir_x, &tgt_dir_y );
	if( tgt_dir_x < 0 ) fld_x += 20*a; else fld_x -= 20*a;
	if( tgt_dir_y < 0 ) fld_y += 15*a; else fld_y -= 15*a;

	_map_grid->Off( pos_his.LastX(), pos_his.LastY() );
	pos_his.Reset( fld_x, fld_y );
	_map_grid->On ( fld_x, fld_y );
	_x        = pos_his.LastX_Real();
	_y        = pos_his.LastY_Real();
}

void FldUnit::SetUnit_Venger( NPU_MapGrid   *map_grid, const FldUnit *tgt_unit )
{
	_tgt_unit   = tgt_unit;
	_map_grid   = map_grid;
	_ctrl       = fuCTRL_Venger;
	_param1     = 0;
	_param2     = 0;
	_dir_x      = 0;
	_dir_y      = 0;
	_beat       = 0;
	_speed      = 1;
	_act_count  = 0;
	_old_param2 = 0;

	_Venger_ResetPosition( 1 );

	_pane_body  ->SetType_Image( 0,  0, MAPPARTS_W, MAPPARTS_H, SfNo_fuVenger );
	Status_Set( fuSTAT_Idle );
	_pane_body  ->SetAnimationGap( _ran.Get( 0, 15 ) );
	_pane_shadow->Visible( false );
}


void FldUnit::SetUnit_Server
(
	unsigned char   gene_count ,
	const  POSSHORT *p_pos_his ,
	char            pos_his_num,
	char            status     ,
	char            param1     ,
	char            param2     ,
	fuCTRL          ctrl )
{
	bool b_new = false;

	if( _status == fuSTAT_Disable || gene_count != _gene_count ) b_new = true;

	if( b_new )
	{
		pos_his.Reset( &p_pos_his[ pos_his_num - 1 ] );
		_x = pos_his.LastX_Real();
		_y = pos_his.LastY_Real();
	}
	else
	{
		for( int h = 0; h < pos_his_num; h++ ) pos_his.PushPos( &p_pos_his[ h ] );
	}


	if( b_new )
	{
		int surf_no;
		switch( ctrl )
		{
		case fuCTRL_Other :
			_seno_scream = SeNo_Scream1 ;
			surf_no      = SfNo_fuPlayer;
			break;

		case fuCTRL_NPU   :

			if( param1 == NPU_Chase )
				param1 = param1;

			switch( param1 )
			{
			case NPU_Goal   : _seno_scream = SeNo_Goal   ; break;

			case NPU_Sword  :
			case NPU_Shield : _seno_scream = SeNo_Equip  ; break;

			case NPU_Closer :
			case NPU_Crawl  :
			case NPU_Wuss   :
			case NPU_Chase  :
			case NPU_Gate   :
			case NPU_Snail  :
				_seno_scream = SeNo_Scream2;
				break;

			case NPU_Cross  : _act_count = 0; break;

			case NPU_Hummer :
			case NPU_Rounder:

				_seno_scream = SeNo_Scream2;
				switch( param2 % 4 )
				{
				case 0: _act_count =   0; break;
				case 1: _act_count =  64; break;
				case 2: _act_count = 128; break;
				case 3: _act_count = 192; break;
				}
				break;


				break;
			}
			surf_no      = SfNo_fuNPU  ;

			break;

		case fuCTRL_Venger:
			_seno_scream = 0;
			surf_no      = SfNo_fuVenger;
			break;
		}

		_pane_body  ->SetType_Image( 0,  0, MAPPARTS_W, MAPPARTS_H, surf_no  );

		_param1  = param1; // color | npu_type
		_param2  = param2; // equip | npu_param
		_ctrl    = ctrl  ;
		_pane_shadow->Visible( true );
		Status_Set( gene_count, status );

		_pane_body  ->SetAnimationGap( _ran.Get( 0, 15 ) );
	}
	else
	{
		_param1 = param1; // color | npu_type
		_param2 = param2; // equip | npu_param
		_ctrl   = ctrl  ;
		Status_Set( gene_count, status );
		Status_Set( gene_count, status );
	}

	_dir_x               = 0;
	_dir_y               = 0;
	_beep_interval_count = 0;
}

void FldUnit::SetPosition_Side()
{
	pos_his.ShiftPush( _dir_x, _dir_y );
	if( _status != fuSTAT_Ghost ) SEFF_Voice_Play( SeNo_Walk );
}



void FldUnit::_Act_Own( const BUTTONSSTRUCT* p_btns )
{
	char lct_x, lct_y;
	bool b_block = false;

	_map->Location_Get( &lct_x, &lct_y );

	unsigned char mparts;


	if( _beep_interval_count ) _beep_interval_count--;

	if( _x == pos_his.LastX_Real() && _y == pos_his.LastY_Real() )
	{
		if     ( !p_btns ) return;
		else if( p_btns->btn & KEY_BUTTON_LEFT  ){ _dir_x = -1; _dir_y =  0; }
		else if( p_btns->btn & KEY_BUTTON_UP    ){ _dir_x =  0; _dir_y = -1; }
		else if( p_btns->btn & KEY_BUTTON_RIGHT ){ _dir_x =  1; _dir_y =  0; }
		else if( p_btns->btn & KEY_BUTTON_DOWN  ){ _dir_x =  0; _dir_y =  1; }
		else return;

		if( _status == fuSTAT_Ghost )
		{
			_speed = 2;
		}
		else
		{

#ifdef _DEBUG
			_speed = 2;
#else
			_speed = 2;
#endif

			mparts = _map->Attribute_Get(
				pos_his.LastX() - lct_x * ROOMGRIDNUM_H + _dir_x,
				pos_his.LastY() - lct_y * ROOMGRIDNUM_V + _dir_y );

			if( _param2 & EQUIPFLAG_SHIELD )
			{
				if( mparts == mPartAttr_Block   ||
					mparts == mPartAttr_bPlayer ||
					mparts == mPartAttr_bNPU    ) b_block = true;
			}
			else
			{
				if( mparts == mPartAttr_Block   ||
					mparts == mPartAttr_bPlayer ) b_block = true;
			}

			if( b_block )
			{
				if( !_beep_interval_count )
				{
					SEFF_Voice_Play( SeNo_Error );
					_beep_interval_count = 10;
				}
				return;
			}
			SEFF_Voice_Play( SeNo_Walk );
		}

		pos_his.ShiftPush( _dir_x, _dir_y );
	}

	if     ( _x < pos_his.LastX_Real() ) _x += VS * _speed;
	else if( _x > pos_his.LastX_Real() ) _x -= VS * _speed;
	else if( _y < pos_his.LastY_Real() ) _y += VS * _speed;
	else if( _y > pos_his.LastY_Real() ) _y -= VS * _speed;
	SEFF_SetHear( pos_his.LastX(), pos_his.LastY() );
}

void FldUnit::_Act_Other()
{
	if( _status == fuSTAT_Disable ) return;
	if     ( _x < pos_his.NextX_Real()) _x += VS * 2;
	else if( _x > pos_his.NextX_Real()) _x -= VS * 2;
	else if( _y < pos_his.NextY_Real()) _y += VS * 2;
	else if( _y > pos_his.NextY_Real()) _y -= VS * 2;
	else pos_his.PopOne();
}

void FldUnit::_Act_NPU()
{
	if( _status == fuSTAT_Disable ) return;

	switch( _param1 )
	{
	case NPU_Closer :
	case NPU_Crawl  :
	case NPU_Wuss   :
	case NPU_Chase  :
	case NPU_Snail  :

		if     ( _x < pos_his.NextX() * VS * MAPPARTS_W ) _x += VS * 2;
		else if( _x > pos_his.NextX() * VS * MAPPARTS_W ) _x -= VS * 2;
		else if( _y < pos_his.NextY() * VS * MAPPARTS_H ) _y += VS * 2;
		else if( _y > pos_his.NextY() * VS * MAPPARTS_H ) _y -= VS * 2;
		else pos_his.PopOne();
		break;

	case NPU_Sword  :
	case NPU_Shield :
	case NPU_Goal   :

		pos_his.PopOne();
		_x = pos_his.NextX() * VS * MAPPARTS_W;
		_y = pos_his.NextY() * VS * MAPPARTS_H;
		break;

	case NPU_Hummer :
	case NPU_Rounder:

		pos_his.PopOne();
		_x = pos_his.NextX() * VS * MAPPARTS_W + g_tri->GetCos( (unsigned char)_act_count ) * 18;
		_y = pos_his.NextY() * VS * MAPPARTS_H + g_tri->GetSin( (unsigned char)_act_count ) * 16;
		if( _param1 == NPU_Hummer ) _act_count = (_act_count + 4 ) & 0xff;
		else                        _act_count = (_act_count - 4 ) & 0xff;
		break;

	case NPU_Gate :
	case NPU_Cross:

		pos_his.PopOne();

		int off_x;
		int off_y;

		off_x = 0;
		off_y = 0;

		if( _old_param2 != _param2 ){ _old_param2  = _param2; _act_count = 0; }

		switch( _param2 )
		{
		case 0: off_x =      _act_count; break; // open  H
		case 1: off_x = 16 - _act_count; break; // close H
		case 2: off_y =      _act_count; break; // open  V
		case 3: off_y = 16 - _act_count; break; // close V
		}
		
		_ani_count++;
		if( (_ani_count /2)% 2 ){ off_x *= -1; off_y *= -1; }

		_x = ( pos_his.NextX() * MAPPARTS_W + off_x ) * VS;
		_y = ( pos_his.NextY() * MAPPARTS_H + off_y ) * VS;

		_act_count++; if( _act_count > 16 ) _act_count = 16;
		break;
	}
}

int _Absolute( int a ){ if( a < 0 ) return -a; return a; }
typedef struct{ char dir_x; char dir_y; }_DIRCHAR;
enum  _DIRECTION                             { _DIR_LEFT = 0, _DIR_UP   , _DIR_RIGHT, _DIR_DOWN , _DIR_num  , };
static const _DIRCHAR _dir_tbl[ _DIR_num ] = { { -1,  0 },    {  0, -1 }, {  1,  0 }, {  0,  1 }              };

void FldUnit::_Act_Venger()
{
	++_act_count;
	if( _act_count >= 60*10 && !(_act_count % (60*10)) ) _Venger_ResetPosition(-1);

	if( _x == pos_his.LastX_Real() && _y == pos_his.LastY_Real() )
	{
		short      npx  =            pos_his.LastX();
		short      npy  =            pos_his.LastY();
		short      plx  = _tgt_unit->pos_his.LastX();
		short      ply  = _tgt_unit->pos_his.LastY();

		_DIRECTION dir  = _DIR_num;
		_DIRECTION dir2 = _DIR_num;

		if( _Absolute( plx - npx ) > _Absolute( ply - npy ) )
		{
			if     ( plx < npx ) dir  = _DIR_LEFT ;
			else if( plx > npx ) dir  = _DIR_RIGHT;
			if     ( ply < npy ) dir2 = _DIR_UP   ;
			else if( ply > npy ) dir2 = _DIR_DOWN ;
		}
		else
		{
			if     ( ply < npy ) dir  = _DIR_UP   ;
			else if( ply > npy ) dir  = _DIR_DOWN ;
			if     ( plx < npx ) dir2 = _DIR_LEFT ;
			else if( plx > npx ) dir2 = _DIR_RIGHT;
		}

		if( dir != _DIR_num )
		{
			if( _map_grid->IsOn( npx + _dir_tbl[ dir ].dir_x, npy + _dir_tbl[ dir ].dir_y ) )
			{
				if( dir2 != _DIR_num && !_map_grid->IsOn( npx + _dir_tbl[ dir2 ].dir_x, npy + _dir_tbl[ dir2 ].dir_y ) )
				{
					dir = dir2;
				}
				else
				{
					int i;
					for( i = 0; i < _DIR_num; i++ )
					{
						if( !_map_grid->IsOn( npx + _dir_tbl[ i ].dir_x, npy + _dir_tbl[ i ].dir_y ) ) break;
					}
					dir = (_DIRECTION)i;
				}
			}
			if( dir != _DIR_num )
			{
				if     ( _act_count > 60 * 60 ) _speed = 2; // 1ï™å„ÇÕïKÇ∏Åc
				else if( npx < plx -  9 || npx > plx +  9 || npy < ply - 6 || npy > ply + 6 ) _speed = _ran.Get( 0, 1 ) ? 1 : 4;
				else if( npx < plx -  3 || npx > plx +  3 || npy < ply - 2 || npy > ply + 2 ) _speed = _ran.Get( 0, 1 ) ? 1 : 2;
				else                                                                          _speed = 1;

				_map_grid->Off( npx, npy );
				_map_grid->On ( npx + _dir_tbl[ dir ].dir_x, npy + _dir_tbl[ dir ].dir_y );
				pos_his.ShiftPush(    _dir_tbl[ dir ].dir_x,       _dir_tbl[ dir ].dir_y );
			}
		}
	}

	if     ( _x < pos_his.LastX_Real() ) _x += VS * _speed;
	else if( _x > pos_his.LastX_Real() ) _x -= VS * _speed;
	else if( _y < pos_his.LastY_Real() ) _y += VS * _speed;
	else if( _y > pos_his.LastY_Real() ) _y -= VS * _speed;
}

void FldUnit::Action( const BUTTONSSTRUCT* p_btns )
{
	if( _status == fuSTAT_Disable ) return;

	switch( _ctrl )
	{
	case fuCTRL_Own   : _Act_Own   ( p_btns ); break;
	case fuCTRL_Other : _Act_Other (        ); break;
	case fuCTRL_NPU   : _Act_NPU   (        ); break;
	case fuCTRL_Venger: _Act_Venger(        ); break;
	}
}

void FldUnit::GetDirection( char *p_dir_x, char *p_dir_y ) const
{
	if( p_dir_x ) *p_dir_x = _dir_x;
	if( p_dir_y ) *p_dir_y = _dir_y;
}

char FldUnit::GetColor(){ return (char)_param1; }

bool FldUnit::OwnHitVenger( FldUnit **p_vens, int ven_num, int *p_index )
{
	if( _status != fuSTAT_Idle    ) return false;

	for( int v = 0; v < ven_num; v++ )
	{
		FldUnit *p_ven = p_vens[ v ];
		if( p_ven->IsAlive() && p_ven->_x == _x && p_ven->_y == _y )
		{
			*p_index = v;
			return true;
		}
	}
	return false;
}

bool FldUnit::OwnHitNPU( FldUnit **p_npus, int npu_num, int *p_index )
{
	if( _status != fuSTAT_Idle ) return false;

	bool b_check;
	bool b_hit = false;

	for( int n = 0; n < npu_num; n++ )
	{
		FldUnit *p_npu = p_npus[ n ];

		b_check = true;

		if(                               p_npu->NPU_IsSnail () ) b_check = false;
		if( _param2 & EQUIPFLAG_SHIELD && p_npu->NPU_IsFire  () ) b_check = false;
		if( _param2 & EQUIPFLAG_SWORD  && p_npu->NPU_IsSword () ) b_check = false;
		if( _param2 & EQUIPFLAG_SHIELD && p_npu->NPU_IsShield() ) b_check = false;

		if( b_check                &&
			p_npu->IsAlive()       &&
			p_npu->_x >= _x - 6*VS &&
			p_npu->_x <  _x + 6*VS &&
			p_npu->_y >= _y - 6*VS &&
			p_npu->_y <  _y + 6*VS )
		{
			if( !_b_hover_npu )
			{
				_b_hover_npu = true;
				*p_index = n;
				return true;
			}
			b_hit = true;
		}
	}
	
	if( !b_hit ) _b_hover_npu = false;
	return false;
}


void FldUnit::SetOwnerStatus( char status, char param1, char param2, bool b_rain )
{
	if( _ctrl != fuCTRL_Own ) return;

	if( _status != fuSTAT_Goal && status == fuSTAT_Goal ) g_grec->Set( _param2 );
	if( !(_param2 & EQUIPFLAG_CROWN) && param2 & EQUIPFLAG_CROWN ) SEFF_Voice_Play( SeNo_Equip );

	Status_Set( status );
	_param1 = param1;

	switch( status )
	{
	case fuSTAT_Ghost:

		BGM_Play( BGM_Suiteki );
		break;

	case fuSTAT_Idle :

		if( param2 & EQUIPFLAG_SWORD )
		{
			BGM_Play( BGM_Kousin );
		}
		else
		{
			if( b_rain ) BGM_Play( BGM_RainDrop );
			else         BGM_Play( BGM_Soap     );
		}
		break;
	}

	_param2 = param2;
}


bool FldUnit::CanDrawFloor()
{
	char lct_x, lct_y;
	_map->Location_Get( &lct_x, &lct_y );
	unsigned char mparts = _map->GetMparts(
				pos_his.LastX(),
				pos_his.LastY() );

//	dlog( "floor mparts: %d", mparts );

	if( mparts < 0x20 && (mparts&0x0f) >= 12 ) return true;
	return false;
}

bool FldUnit::NPU_IsFire()
{
	if( _ctrl != fuCTRL_NPU ) return false;
	switch( _param1 )
	{
	case NPU_Cross  :
	case NPU_Gate   :
	case NPU_Hummer :
	case NPU_Rounder: return true;
	}
	return false;
}

bool FldUnit::NPU_IsShield()
{
	if( _ctrl   != fuCTRL_NPU ) return false;
	if( _param1 != NPU_Shield ) return false;
	return true;
}

bool FldUnit::NPU_IsSword()
{
	if( _ctrl   != fuCTRL_NPU ) return false;
	if( _param1 != NPU_Sword  ) return false;
	return true;
}

bool FldUnit::NPU_IsSnail()
{
	if( _ctrl   != fuCTRL_NPU ) return false;
	if( _param1 != NPU_Snail  ) return false;
	return true;
}
