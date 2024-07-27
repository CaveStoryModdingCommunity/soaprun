#include "utility_Input/Buttons.h"

#include "PosHistory.h"
#include "NPU_MapGrid.h"
#include "Map.h"


enum fuCTRL
{
	fuCTRL_None  ,
	fuCTRL_Own   ,
	fuCTRL_Other ,
	fuCTRL_NPU   ,
	fuCTRL_Venger,
};

class FldUnit
{
	Pane           *_pane_shadow;
	Pane           *_pane_body  ;
	Pane           *_pane_sword ;
	Pane           *_pane_crown ;
	Pane           *_pane_shield;
				 
	const Map      *_map        ;
	NPU_MapGrid    *_map_grid   ;
	const FldUnit  *_tgt_unit   ;
				 
	int            _x           ;
	int            _y           ;
	char           _dir_x       ;
	char           _dir_y       ;
	char           _status      ;
	char           _param1      ; // color_index / npu_type
	char           _param2      ; // equip       / ..
	char           _beep_interval_count;
	char           _beat        ;
	char           _speed       ;
	unsigned char  _gene_count  ; // サーバーから取得する識別用の生成カウント
	fuCTRL         _ctrl        ;

	short          _act_count   ;
	unsigned char  _ani_count   ;
	char           _old_param2  ;

	char           _seno_scream ;
	bool           _b_hover_npu ;

	void _Act_Own   ( const BUTTONSSTRUCT* p_btns );
	void _Act_Other ();
	void _Act_NPU   ();
	void _Act_Venger();

	void _Venger_ResetPosition( int a );

public:

	PosHistory pos_his;

	FldUnit( const Map *map, bool b_equip );
	~FldUnit();

	void Disable();

	void SetUnit_Owner(
			short  fld_x , short fld_y ,
			char   dir_x , char  dir_y );


	void SetUnit_Server(
			unsigned char  gene_count ,
			const POSSHORT *p_pos_his ,
			char           pos_his_num,
			char           status     ,
			char           param1     ,
			char           param2     ,
			fuCTRL         ctrl );

	void SetUnit_Venger( NPU_MapGrid   *map_grid, const FldUnit *tgt_unit );

	bool OwnHitNPU   ( FldUnit **p_npus, int npu_num, int *p_index );
	bool OwnHitVenger( FldUnit **p_vens, int ven_num, int *p_index );

	void Action      ( const BUTTONSSTRUCT* p_btns );
	bool GetScroll   ();
	bool IsEnable    () const;
	bool IsAlive     () const;
				  
	bool IsIdle      () const;
	bool IsDead      () const;
	bool IsGoal      () const;
	bool IsGhost     () const;
				  
	bool IsSword     () const;

	void SetPosition_Side();

	void GetDirection( char *p_dir_x, char *p_dir_y ) const;
	char GetColor    ();

	Pane *Pane_body  ();
	Pane *Pane_shadow();
	void  Pane_Update();

	bool Status_Set  ( unsigned char gene_count, char status );
	bool Status_Set  (                           char status );

	void SetOwnerStatus( char status, char param1, char param2, bool b_rain );

	bool CanDrawFloor();

	bool NPU_IsFire  ();
	bool NPU_IsShield();
	bool NPU_IsSword ();
	bool NPU_IsSnail ();
};
