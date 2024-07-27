#include <StdAfx.h>

#include "../Fixture/DebugLog.h"
#include "../Fixture/Random.h"

#include "../Dx/DxDraw.h"

#include "utility_Draw/FontWork.h"
#include "utility_Draw/ScreenCurtain.h"
#include "utility_Draw/PxImage.h"
#include "utility_Draw/ListFrame.h"
#include "utility_Draw/SecondCounter.h"

#include "utility_Sound/SEFF.h"
#include "utility_Sound/BGM.h"
#include "utility_Input/Buttons.h"

#include "scene_Buttons.h"
#include "Scene.h"
#include "ColorSelection.h"
#include "DrawSelection.h"
#include "Map.h"
#include "FldUnit.h"
#include "TalkBoard.h"
#include "RequestResponse.h"
#include "loop.h"
#include "Inquiry.h"
#include "Weather.h"
#include "GameRecord.h"

#define _MAPNAME_X (FIELDVIEW_W + 4)
#define _MAPNAME_Y (4)

#define _REQUEST_INTERVAL 14

#define _MAX_VENGER 12

enum _GAMEPHASE
{
	_GAMEPHASE_Loading =   0  ,
	_GAMEPHASE_Walk           ,
	_GAMEPHASE_CheckLocation  ,
	_GAMEPHASE_Scroll         ,
	_GAMEPHASE_Menu           ,
	_GAMEPHASE_Color          ,
	_GAMEPHASE_Draw           ,
	_GAMEPHASE_Dead           ,
	_GAMEPHASE_Goal           ,
	_GAMEPHASE_Inquiry_Suspend,
	_GAMEPHASE_Inquiry_Wait   ,
	_GAMEPHASE_num            ,
};

static char *_phase_msgs[] =
{
	"Loading.."      ,
	"Walk"           ,
	"Check Location" ,
	"Scroll"         ,
	"Menu"           ,
	"Color"          ,
	"Draw"           ,
	"Dead"           ,
	"Goal"           ,
	"Inauiry Suspend",
	"Inauiry Wait"   ,
	"-"              ,
};

extern DWORD          g_color_bg_field;
extern DWORD          g_color_bg_text ;
extern DWORD          g_color_text   ;

extern RECT           g_rc_wnd    ;
extern RECT           g_rc_fld    ;
extern RECT           g_rc_txt    ;
extern Scene          *g_scene    ;
extern RingBuffer     *g_ring_msg ;
extern RingBuffer     *g_ring_req ;
extern RingBuffer     *g_ring_res ;
extern TalkBoard      *g_talkboard;
extern GameRecord     *g_grec     ;

extern Random         g_ran       ;

static FldUnit        *_uni_own     = NULL;
static FldUnit        *_uni_oths[ MAX_OTHERUNIT ];
static FldUnit        *_uni_npus[ MAX_NPU       ];
static FldUnit        *_uni_vens[ _MAX_VENGER   ];

static NPU_MapGrid    *_grid_gho    = NULL;

static Map            *_map         = NULL;
static Pane           *_pane_frame1 = NULL;
static Pane           *_pane_fld    = NULL;

static ColorSelection *_colorsel    = NULL;
static DrawSelection  *_drawsel     = NULL;
static ScreenCurtain  *_sctn        = NULL;
static Weather        *_wt          = NULL;

static char           *_buf_res     = NULL;

#define _MENU_NUM 2

static ListFrame      *_lf_menu     = NULL;
static SIMPLELIST     _ls_menu[ _MENU_NUM ] =
{
	{ _GAMEPHASE_Color, "‚¢‚ë‚ª‚¦", NULL, false },
	{ _GAMEPHASE_Draw , "‚Ê‚è"    , NULL, false },
};

static SecondCounter  _sec( 3 );


static _GAMEPHASE     _phase             = _GAMEPHASE_Loading;
static int            _phase_count       =     0;

static char           _req_field_count       =     0;
static bool           _b_wait_field_response = false;

//
// local class -----
//
class FPSLogger
{
private:

	enum{ _LOG_NUM = 4 };

	int          _interval        ;
	unsigned int _logs[ _LOG_NUM ];
	int          _w               ;

public:

	void Reset(){ _interval = 0; memset( _logs, 0, sizeof(int) * _LOG_NUM ); _w = 0; }
	FPSLogger(){ Reset(); }
	void RequestDlog(){ Request_Dlog( "fps %d,%d,%d,%d", _logs[ 0 ],_logs[ 1 ],_logs[ 2 ],_logs[ 3 ] ); }

	void Procedure()
	{
		if( ++_interval > 180 ){ _logs[ _w ] = Loop_GetFPS(); _w++; if( _w > _LOG_NUM ) _w = 0; _interval = 0; }
	}
};

class _MessageBoard
{
private:
	int _x, _y, _offset_y;
	const char *_p_str;

public:
	_MessageBoard( int x, int y ){ _x = x; _y = y; _p_str = NULL; }
	void Set( const char *p_str ){ _p_str = p_str; _offset_y = 0; }
	void Action(){ if( _offset_y > 0 ) _offset_y--; }
	void Put( const RECT *p_rc_view ){ if( _p_str ) FontWork_PutText( p_rc_view, _x, _y + _offset_y, _p_str, 0 ); }
};

static FPSLogger     _fps_logger  ;
static _MessageBoard _msg( 14, 4 );

//
// -----
//

class _CallVenger
{
private:

	bool _b_ON     ;
	int  _reprieve ;
	int  _gho_count;
	int  _wait     ;

public :

	void Reset()
	{
		_b_ON      = 0;
		_reprieve  = 0;
		_gho_count = 0;
		_wait      = 0;
		for( int g = 0; g < _MAX_VENGER; g++ ) _uni_vens[ g ]->Disable();
	}

	_CallVenger()
	{
		Reset();
	}

	void Switch( bool b )
	{
		if( _b_ON && !b ) Reset();
		_b_ON = b;
	}

	void Procedure()
	{
		if( !_b_ON ) return;
		if( _reprieve < 60 * 30 && 1 )
		{
			_reprieve++;
		}
		else if( _gho_count < _MAX_VENGER ) 
		{
			if( ++_wait > 60 * 6 ) // 6•b–ˆoŒ»
			{
				_wait = 0;
				_uni_vens[ _gho_count ]->SetUnit_Venger( _grid_gho, _uni_own );
				_gho_count++;
			}
		}
	}
};

static _CallVenger *_call_venger = NULL;



static void _SetPhase( _GAMEPHASE phase, int se_no )
{
	_phase       = phase;
    _phase_count =     0;
	SEFF_Voice_Play( se_no );
	_msg.Set( _phase_msgs[ phase ] );

	switch( phase )
	{
	case _GAMEPHASE_Menu           :
		_colorsel->Close();
		_drawsel ->Close();
		_lf_menu->SetVisible( true );
		break;

	case _GAMEPHASE_Color          : _colorsel->Open( _uni_own->GetColor() ); break;
	case _GAMEPHASE_Draw           : _drawsel ->Open(                      ); break;

	case _GAMEPHASE_Walk           :
	case _GAMEPHASE_Loading        :
	case _GAMEPHASE_CheckLocation  :
	case _GAMEPHASE_Scroll         :
	case _GAMEPHASE_Dead           :
	case _GAMEPHASE_Goal           :
	case _GAMEPHASE_Inquiry_Suspend:
	case _GAMEPHASE_Inquiry_Wait   :

		_colorsel->Close();
		_drawsel ->Close();
		_lf_menu ->SetVisible( false );
		break;
	}

	_sec.Reset();
}


////
//
// Request..
//
////


static bool _PushFieldRequest( int req_size )
{
	char pos_his_num = _uni_own->pos_his.Pop( (POSSHORT*)( &_buf_res[ req_size ] ) );
	_buf_res[ req_size - 1 ] = pos_his_num;
	if( !g_ring_req->bin_Push( _buf_res, req_size + pos_his_num * sizeof(POSSHORT) ) )
	{
		dlog( "can't req push: %c%c%c%c", _buf_res[ 0 ], _buf_res[ 1 ], _buf_res[ 2 ], _buf_res[ 3 ] ); return false;
	}
	_req_field_count       =    0;
	_b_wait_field_response = true;
	return true;
}

static bool _Request_MyPosition()
{
	if( _b_wait_field_response ) return false;
	reqMYPOSITION *p_req = (reqMYPOSITION*)_buf_res;
	memcpy( p_req->req_name, g_req_names[ clREQUEST_MyPosition ], BUFFERSIZE_REQNAME );
	return _PushFieldRequest( sizeof(reqMYPOSITION) );
}

class _Inquiry
{
private:

	enum _INQTYPE
	{
		_INQ_None       ,
		_INQ_HitNPU     ,
		_INQ_HitVenger  ,
		_INQ_ChangeColor,
		_INQ_DrawFloor  ,
	};

	_INQTYPE      _type  ;
	unsigned char _tgt   ;
	char          _param1;
	short         _fld_x ;
	short         _fld_y ;
	unsigned char _i     ;

public :

	_Inquiry()
	{
		_type = _INQ_None;
	}

	void Reset()
	{
		_type = _INQ_None;
	}

	void Set_HitNPU( unsigned char tgt )
	{
		_type = _INQ_HitNPU       ;
		_tgt  = tgt               ;
	}

	void Set_HitVenger()
	{
		_type = _INQ_HitVenger    ;
	}

	void Set_ChangeColor( char param1 )
	{
		_type   = _INQ_ChangeColor;
		_param1 = param1          ;
	}

	void Set_DrawFloor( short fld_x, short fld_y, unsigned char i )
	{
		_type  = _INQ_DrawFloor   ;
		_fld_x = fld_x            ;
		_fld_y = fld_y            ;
		_i     = i                ;
	}

	bool _Try_Request_Inquiry()
	{
		if( _b_wait_field_response ) return false;

		bool b = false;

		switch( _type )
		{
		case _INQ_HitNPU     :
			{
				reqINQ_HitNPU *p_req = (reqINQ_HitNPU*)_buf_res;
				memcpy( p_req->req_name, g_req_names[ clREQUEST_INQ_HitNPU ], BUFFERSIZE_REQNAME );
				p_req->uc_tgt      = _tgt;
				b = _PushFieldRequest( sizeof(reqINQ_HitNPU) );
			}
			break;
		case _INQ_HitVenger  :
			{
				reqINQ_HitVenger *p_req = (reqINQ_HitVenger*)_buf_res;
				memcpy( p_req->req_name, g_req_names[ clREQUEST_INQ_HitVenger ], BUFFERSIZE_REQNAME );
				b = _PushFieldRequest( sizeof(reqINQ_HitVenger) );
			}
			break;
		case _INQ_ChangeColor:
			{
				reqINQ_ChangeColor *p_req = (reqINQ_ChangeColor*)_buf_res;
				memcpy( p_req->req_name, g_req_names[ clREQUEST_INQ_ChangeColor ], BUFFERSIZE_REQNAME );
				p_req->param1 = _param1;
				b = _PushFieldRequest( sizeof(reqINQ_ChangeColor) );
			}
			break;
		case _INQ_DrawFloor  :
			{
				reqINQ_DrawFloor *p_req = (reqINQ_DrawFloor*)_buf_res;
				memcpy( p_req->req_name, g_req_names[ clREQUEST_INQ_DrawFloor ], BUFFERSIZE_REQNAME );
				p_req->fld_x  = _fld_x;
				p_req->fld_y  = _fld_y;
				p_req->i      = _i    ;
				b = _PushFieldRequest( sizeof(reqINQ_DrawFloor) );
			}
			break;
		}
		if( b ) _type = _INQ_None;

		return b;
	}
};

static _Inquiry *_inq = NULL;

void scene_Game_Reset( void )
{
	_inq       ->Reset();
	g_talkboard->Clear();
	g_ring_msg ->Clear();
	g_ring_req ->Clear();
	g_ring_res ->Clear();
	_b_wait_field_response = false;

	reqMAPATTRIBUTE req;
	memcpy( req.req_name, g_req_names[ clREQUEST_MapAttribute ], BUFFERSIZE_REQNAME );
	if( !g_ring_req ->bin_Push( (void*)&req, sizeof(reqMAPATTRIBUTE) ) ){ dlog( "mAtt push err!" ); return; }

	_uni_own->Disable();
	for( int u = 0; u < MAX_OTHERUNIT; u++ ) _uni_oths[ u ]->Disable();
	for( int u = 0; u < MAX_NPU      ; u++ ) _uni_npus[ u ]->Disable();

	_call_venger->Reset();

	char lct_x = 1;
	char lct_y = 1;
	_uni_own->SetUnit_Owner(
		ROOMGRIDNUM_H * lct_x + 10,
		ROOMGRIDNUM_V * lct_y +  7,
		0, 1 ); // param

	_grid_gho->Clear();
	_map     ->Location_Set( lct_x, lct_y );
	_sctn    ->Mask();

	_Request_MyPosition();

	_sec.Reset        ();
	_fps_logger.Reset ();
	_SetPhase( _GAMEPHASE_Loading, 0 );

	_colorsel->Close();
	_drawsel ->Close();
}



bool scene_Game_Initialize( void )
{
	bool b_ret = false;

	// Load Graphics
	if( !PxImage_Load( "mparts"   , SfNo_MapParts, TRUE ) ) goto End;
	if( !PxImage_Load( "shadow"   , SfNo_Shadow  , TRUE ) ) goto End;
	if( !PxImage_Load( "funit-pla", SfNo_fuPlayer, TRUE ) ) goto End;
	if( !PxImage_Load( "funit-npu", SfNo_fuNPU   , TRUE ) ) goto End;
	if( !PxImage_Load( "funit-ven", SfNo_fuVenger, TRUE ) ) goto End;
	if( !PxImage_Load( "equip"    , SfNo_Equip   , TRUE ) ) goto End;
	if( !PxImage_Load( "weather"  , SfNo_Weather , TRUE ) ) goto End;
	if( !PxImage_Load( "end-snaps", SfNo_EndSnaps, TRUE ) ) goto End;

	if( !( _buf_res = (char*)malloc( BUFFERSIZE_ATTENDER ) ) ) goto End;

	_map         = new Map ();


	_pane_fld    = new Pane( 1 + (1 + MAX_OTHERUNIT + MAX_NPU) * 2 + _MAX_VENGER + 1 ); // map + funit * 2 + weather
	_pane_fld->SetType_View( 0, 0, FIELDVIEW_W, FIELDVIEW_H );
	_pane_fld->Child_Add( _map->Pane_Get() );

	_pane_frame1 = new Pane( 3 ); // fld / colorsel / drawsel
	_pane_frame1->SetType_Fill( 0, 0, FIELDVIEW_W, FIELDVIEW_H, DxDraw_GetSurfaceColor( SfNo_COLOR_BGFIELD ) );
	_pane_frame1->Child_Add( _pane_fld );

	for( int u = 0; u < MAX_NPU      ; u++ ){ _uni_npus[ u ] = new FldUnit( _map, false ); _pane_fld->Child_Add( _uni_npus[ u ]->Pane_shadow() ); }
	for( int u = 0; u < MAX_OTHERUNIT; u++ ){ _uni_oths[ u ] = new FldUnit( _map, true  ); _pane_fld->Child_Add( _uni_oths[ u ]->Pane_shadow() ); }
	{                                         _uni_own       = new FldUnit( _map, true  ); _pane_fld->Child_Add( _uni_own      ->Pane_shadow() ); }
	for( int u = 0; u < _MAX_VENGER  ; u++ ){ _uni_vens[ u ] = new FldUnit( _map, false ); }

	for( int u = 0; u < MAX_NPU      ; u++ ){ _pane_fld->Child_Add( _uni_npus[ u ]->Pane_body() ); }
	for( int u = 0; u < MAX_OTHERUNIT; u++ ){ _pane_fld->Child_Add( _uni_oths[ u ]->Pane_body() ); }
	{										  _pane_fld->Child_Add( _uni_own      ->Pane_body() ); }
	for( int u = 0; u < _MAX_VENGER  ; u++ ){ _pane_fld->Child_Add( _uni_vens[ u ]->Pane_body() ); }

	_wt = new Weather( FIELDVIEW_W, FIELDVIEW_H,  12, &g_ran );
	_pane_fld->Child_Add( _wt->Pane_Get() );

	_lf_menu  = new ListFrame( LISTFRAME_LIST, 5, _MENU_NUM, POSFROM_LB, 8, 8, true );
	_lf_menu->List_Set( _ls_menu, _MENU_NUM );

	_colorsel    = new ColorSelection(); _pane_frame1->Child_Add( _colorsel->GetPane() );
	_drawsel     = new DrawSelection (); _pane_frame1->Child_Add( _drawsel ->GetPane() );
	_sctn        = new ScreenCurtain( DxDraw_GetSurfaceColor( SfNo_COLOR_BGFIELD ) );
	_grid_gho    = new NPU_MapGrid( MAX_FIELDSIZE_W, MAX_FIELDSIZE_L );

	_call_venger = new _CallVenger();
	_inq         = new _Inquiry   ();

	b_ret        = true;
End:
	return b_ret;
}

void scene_Game_Release( void )
{  
	if( _map         ) delete _map        ; _map         = NULL;
	if( _grid_gho    ) delete _grid_gho   ; _grid_gho    = NULL;
	if( _pane_fld    ) delete _pane_fld   ; _pane_fld    = NULL;
	if( _pane_frame1 ) delete _pane_frame1; _pane_frame1 = NULL;
	if( _uni_own     ) delete _uni_own    ; _uni_own     = NULL;
										   
	if( _call_venger ) delete _call_venger; _call_venger = NULL;

	for( int u = 0; u < MAX_OTHERUNIT; u++ ){ if( _uni_oths[ u ] ) delete _uni_oths[ u ]; }
	for( int u = 0; u < MAX_NPU      ; u++ ){ if( _uni_npus[ u ] ) delete _uni_npus[ u ]; }
	for( int u = 0; u < _MAX_VENGER  ; u++ ){ if( _uni_vens[ u ] ) delete _uni_vens[ u ]; }

	if( _colorsel    ) delete _colorsel   ; _colorsel    = NULL;
	if( _drawsel     ) delete _drawsel    ;	_drawsel     = NULL;
	if( _sctn        ) delete _sctn       ;	_sctn        = NULL;
	if( _inq         ) delete _inq        ;	_inq         = NULL;
				 
	if( _buf_res     ) free( _buf_res )   ; _buf_res     = NULL;
	if( _wt          ) delete _wt         ; _wt          = NULL;
}


static void _Response()
{
	resMAPATTRIBUTE *p_res_matt;
	resROOM         *p_res_room;
	int             count;

	if( g_ring_res->IsPop() )
	{
		int res_size =  g_ring_res->bin_Pop( _buf_res );
		if( !res_size || res_size > BUFFERSIZE_ATTENDER ) dlog( "response size! %d", res_size );

		int r = 0;
		for( ; r < svRESPONSE_num; r++ ){ if( !memcmp( _buf_res, g_res_names[ r ], BUFFERSIZE_RESNAME ) ) break; }

		switch( r )
		{
		case svRESPONSE_MapAttribute:

			p_res_matt = (resMAPATTRIBUTE*)_buf_res;
			_map->Attribute_Set( p_res_matt->w, p_res_matt->l, (unsigned char*)&_buf_res[ sizeof(resMAPATTRIBUTE) ] );
//			g_ring_msg->txt_Push( "LD mAtt %dx%d", p_res_matt->w, p_res_matt->l );
			break;

		case svRESPONSE_Room:

			p_res_room = (resROOM*)_buf_res;
			_map->Room_Set( p_res_room->x, p_res_room->y, (unsigned char*)&_buf_res[ sizeof(resROOM) ] );
//			g_ring_msg->txt_Push( "LD room %d,%d", p_res_room->x, p_res_room->y );
			break;

		case svRESPONSE_Field:

			resFIELD_HEAD   *p_head;
			resFIELD_fUNIT  *p_funi;
			resFIELD_MPARTS *p_mprt;
			int             offset ;

			p_head = (resFIELD_HEAD *)_buf_res;
			p_funi = (resFIELD_fUNIT*)&_buf_res[ sizeof(resFIELD_HEAD) ];

			_uni_own->SetOwnerStatus( p_head->own_status, p_head->own_param1, p_head->own_param2, _wt->IsRain() );

			_wt->Set( (WEATHER)p_head->weather );

			count  = 0;

			for( int u = 0; u < MAX_OTHERUNIT; u++ )
			{
				if( count < p_head->num_other && u == p_funi->index && p_funi->pos_his_num )
				{
					_uni_oths[ u ]->SetUnit_Server(
						p_funi->gene_count             ,
						(const POSSHORT*)( p_funi + 1 ),
						p_funi->pos_his_num            ,
						p_funi->status                 ,
						p_funi->param1                 ,
						p_funi->param2                 ,
						fuCTRL_Other                  );
					offset = sizeof(resFIELD_fUNIT) + sizeof(POSSHORT) * p_funi->pos_his_num;
					p_funi = (resFIELD_fUNIT*)( (char*)p_funi + offset );
					count++;
				}
				else
				{
					_uni_oths[ u ]->Disable();
				}
			}

			count  = 0;

			for( int u = 0; u < MAX_NPU; u++ )
			{
				if( count < p_head->num_npu && u == p_funi->index )
				{
					_uni_npus[ u ]->SetUnit_Server(
						p_funi->gene_count ,
						(const POSSHORT*)( p_funi + 1 ),
						p_funi->pos_his_num,
						p_funi->status     ,
						p_funi->param1     ,
						p_funi->param2     ,
						fuCTRL_NPU         );
					offset = sizeof(resFIELD_fUNIT) + sizeof(POSSHORT) * p_funi->pos_his_num;
					p_funi = (resFIELD_fUNIT*)( (char*)p_funi + offset );
					count++;
				}
				else
				{
					_uni_npus[ u ]->Disable();
				}
			}

			p_mprt = (resFIELD_MPARTS*)p_funi;
			for( int i = 0; i < p_head->num_mparts; i++ )
			{
				_map->SetMparts( p_mprt->x, p_mprt->y, p_mprt->parts );
				p_mprt++;
			}

			_b_wait_field_response = false;
		}
	}
}

static _IsActionEnable()
{
	if( _phase != _GAMEPHASE_Loading         &&
		_phase != _GAMEPHASE_CheckLocation   &&
		_phase != _GAMEPHASE_Scroll          &&
		_phase != _GAMEPHASE_Inquiry_Suspend &&
		_phase != _GAMEPHASE_Inquiry_Wait ) return true;
	return false;
}

static void _Action( const BUTTONSSTRUCT* p_btns )
{
	char dir_x, dir_y;
	char lct_x, lct_y;

	_call_venger->Switch( _uni_own->IsSword() && _phase != _GAMEPHASE_Goal ); 
	_call_venger->Procedure();

	if( _IsActionEnable() )
	{
		for( int u = 0; u < MAX_NPU      ; u++ ) _uni_npus[ u ]->Action( p_btns );
		for( int u = 0; u < MAX_OTHERUNIT; u++ ) _uni_oths[ u ]->Action( p_btns );
		for( int u = 0; u < _MAX_VENGER  ; u++ ) _uni_vens[ u ]->Action( p_btns );
	}

	switch( _phase )
	{
	case _GAMEPHASE_Loading:

		char str[ 20 ];

		sprintf( str, "wait field %d\n", _b_wait_field_response );
		OutputDebugStr( str );

		if( !_map->Location_Check( 0, 0 ) || _b_wait_field_response ) break;

		_sctn->Set_In     ();
		_map ->Pane_Update();
		_SetPhase( _GAMEPHASE_Walk, 0 );

	case _GAMEPHASE_Walk  :

		if     ( _uni_own->IsGoal   () ){ BGM_Stop( 2000 ); _SetPhase( _GAMEPHASE_Goal, 0 ); Request_Dlog( "Goal(%d,%d)", _uni_own->pos_his.LastX(), _uni_own->pos_his.LastY() ); break; }
		else if( _uni_own->IsDead   () ){ BGM_Stop( 2000 ); _SetPhase( _GAMEPHASE_Dead, 0 ); Request_Dlog( "Dead(%d,%d)", _uni_own->pos_his.LastX(), _uni_own->pos_his.LastY() ); break; }
//		else if( _uni_own->IsInquiry() ){ _SetPhase( _GAMEPHASE_Inquiry_Suspend, 0 ); break; }
		else if( _uni_own->GetScroll() ){ _SetPhase( _GAMEPHASE_CheckLocation  , 0 ); break; }

		if( _uni_own->IsIdle() && p_btns->trg & KEY_BUTTON_1 )
		{
			_lf_menu->SetVisible( true );
			_SetPhase( _GAMEPHASE_Menu, SeNo_Ok );
			break;
		}

		_uni_own->Action   ( p_btns );
		break;

	case _GAMEPHASE_CheckLocation:

		_uni_own ->GetDirection  ( &dir_x,   &dir_y   );
		if( !_map->Location_Check(  dir_x,    dir_y   ) ) break;

		_map     ->Location_Shift(  dir_x,    dir_y   );
		_pane_fld->SetScrollIn   ( -dir_x*8, -dir_y*8 );

		_map     ->Location_Get  ( &lct_x  , &lct_y   );

		_SetPhase( _GAMEPHASE_Scroll, 0 );

	case _GAMEPHASE_Scroll:

		_uni_own->Action( NULL );

		if( _pane_fld->IsAnimation() ) break;
		_pane_fld->SetPos( 0, 0 );
		_uni_own ->SetPosition_Side();
		_SetPhase( _GAMEPHASE_Walk, 0 );
		break;

	case _GAMEPHASE_Menu:

		if     ( p_btns->trg & KEY_BUTTON_1 )
		{
			_lf_menu->List_Set_CursorFix();
			_GAMEPHASE ph = (_GAMEPHASE)_lf_menu->List_Get_CursorParam();
			if( ph == _GAMEPHASE_Draw && !_uni_own->CanDrawFloor() )
			{
				SEFF_Voice_Play( SeNo_Error );
			}
			else
			{
				_SetPhase( ph, SeNo_Ok );
			}
			break;
		}
		else if( p_btns->trg & KEY_BUTTON_2 ){ _SetPhase( _GAMEPHASE_Walk, SeNo_Cancel ); break; }

		_lf_menu->Action( p_btns );
		break;

	case _GAMEPHASE_Color:

		if( p_btns->trg & KEY_BUTTON_1 )
		{
			_colorsel->Close();
			_inq->Set_ChangeColor( _colorsel->GetSelect() );
			_SetPhase( _GAMEPHASE_Inquiry_Suspend, SeNo_Ok );
			break;
		}
		else if( p_btns->trg & KEY_BUTTON_2 ){ _SetPhase( _GAMEPHASE_Menu, SeNo_Cancel );  break; }
		_colorsel->Action( p_btns );
		break;

	case _GAMEPHASE_Draw:

		if( p_btns->trg & KEY_BUTTON_1 )
		{
			_drawsel->Close();
			if( _uni_own->CanDrawFloor() )
			{
				_inq->Set_DrawFloor( _uni_own->pos_his.LastX(), _uni_own->pos_his.LastY(), (char)_drawsel->GetSelect() );
				_SetPhase( _GAMEPHASE_Inquiry_Suspend, SeNo_Ok );
			}
			else
			{
				_SetPhase( _GAMEPHASE_Walk, SeNo_Error );
			}
			break;
		}
		else if( p_btns->trg & KEY_BUTTON_2 ){ _SetPhase( _GAMEPHASE_Menu, SeNo_Cancel ); break; }
		_drawsel->Action( p_btns );
		break;

	case  _GAMEPHASE_Goal:

		if( _phase_count == 60 * 2                    ) _sctn->Set_Out_Center();
		if( _phase_count >  60 * 2 && _sctn->IsIdle() )
		{
			_fps_logger.RequestDlog();
			g_scene->Set( SCENE_Ending );
		}
		break;

	case  _GAMEPHASE_Dead:

		if( _phase_count == 60 * 2                    ) _sctn->Set_Out_Center();
		if( _phase_count >  60 * 2 && _sctn->IsIdle() )
		{
			Request_mCorpse( _uni_own->pos_his.LastX(), _uni_own->pos_his.LastY() );
			_fps_logger.RequestDlog();
			g_scene->Set( SCENE_GameOver );
		}
		break;

	case _GAMEPHASE_Inquiry_Suspend:

		if( _inq->_Try_Request_Inquiry() ) _SetPhase( _GAMEPHASE_Inquiry_Wait, 0 );
		break;

	case _GAMEPHASE_Inquiry_Wait:

		if( _b_wait_field_response ) break;
		_SetPhase( _GAMEPHASE_Walk, 0 );
		break;
	}

	if( _IsActionEnable() )
	{
		int i;
		if(      _uni_own->OwnHitNPU   ( _uni_npus, MAX_NPU    , &i ) )
		{
			_inq->Set_HitNPU( i );
			_SetPhase( _GAMEPHASE_Inquiry_Suspend, SeNo_Inquiry );
		}
		else if( _uni_own->OwnHitVenger( _uni_vens, _MAX_VENGER, &i ) )
		{
			_inq->Set_HitVenger();
			_SetPhase( _GAMEPHASE_Inquiry_Suspend, SeNo_Inquiry );
		}
	}

	if( _phase != _GAMEPHASE_Inquiry_Suspend   &&
		_phase != _GAMEPHASE_Inquiry_Wait      &&
		++_req_field_count >= _REQUEST_INTERVAL ) _Request_MyPosition();

	_uni_own->Pane_Update();
	for( int u = 0; u < MAX_NPU      ; u++ ) _uni_npus[ u ]->Pane_Update();
	for( int u = 0; u < MAX_OTHERUNIT; u++ ) _uni_oths[ u ]->Pane_Update();
	for( int u = 0; u < _MAX_VENGER  ; u++ ) _uni_vens[ u ]->Pane_Update();

	_map        ->TryRequest();
	_pane_frame1->Animation ();

	g_talkboard ->Procedure ();
	_sctn       ->Procedure ();
	_fps_logger.  Procedure ();
	_wt         ->Procedure ();
}

static void _Put( void )
{
	DxDraw_PaintRect( &g_rc_txt, g_color_bg_text  );

	_pane_frame1->Put( &g_rc_fld, 0, 0 );
	_lf_menu    ->Put( &g_rc_wnd       );
	_sctn       ->Put();
	_msg.         Put( &g_rc_fld );
	FontWork_PutText ( &g_rc_wnd, _MAPNAME_X, _MAPNAME_Y, _map->Name(), 0 );
	g_talkboard ->Put( &g_rc_wnd );
	_sec.         Put( &g_rc_wnd, 16, 16 );

	int count = 1;
	for( int u = 0; u < MAX_OTHERUNIT ; u++ ){ if( _uni_oths[ u ]->IsEnable() ) count++; }

	FontWork_PutText   ( &g_rc_wnd, 12, 116, "‚³‚ñ‚©", 0 );
	FontWork_PutNumber6( &g_rc_wnd, 40, 116, count   , 3 );

//	FontWork_PutNumber6( &g_rc_wnd, 40, 128, _pane_frame1->GetTotal(), 3 );

	// ring request..
	RECT rc = { 14, 30, 14 + 32, 38 };
	DxDraw_PaintRect( &rc, g_color_bg_text );
	rc.right = 14 + g_ring_req->GetUsing() * 32 / g_ring_req->GetSize();
	DxDraw_PaintRect( &rc, g_color_text    );
//	FontWork_PutNumber6( &g_rc_wnd, 8, 128, g_ring_req->Get_w(), 4 );
//	FontWork_PutNumber6( &g_rc_wnd, 8, 140, g_ring_req->Get_r(), 4 );
}

void scene_Game_Procedure( const BUTTONSSTRUCT* p_btns )
{
	_Response();
	_Action( p_btns );
//	if( !Loop_IsSkip() )
		_Put();
	_phase_count++;
}
