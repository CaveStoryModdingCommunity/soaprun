#include <StdAfx.h>

#include "../../Dx/DxSound.h"
#include "../pxtone/pxtone.h"

#include "SEFF.h"

static const char   *_ext   = "ptnoise";
static BOOL         _bMute = FALSE    ;

static short _hear_x = 0;
static short _hear_y = 0;

typedef struct
{
	BOOL bOn;
	long no ;

}_NOISESTRUCT;

static _NOISESTRUCT _noise;

bool _Voice_Load( const char *name, long no, bool bExterior )
{
	bool b_ret = false;
	char path[ MAX_PATH ];
	long channel_num, sps, bps;
	const char* type = "ptnoise";

	PXTONENOISEBUFFER* p_noise = NULL;

	if( bExterior ){ sprintf( path, "%s\\%s.%s", g_dir_data, name, _ext ); type = NULL; }
	else           { sprintf( path, "PTN_%s", name ); }
	pxtone_GetQuality( &channel_num, &sps, &bps, NULL );
	if( !( p_noise = pxtone_Noise_Create( path, type,         channel_num, sps, bps )   ) ) goto End;
	if( !DxSound_Voice_Create( p_noise->p_buf, p_noise->size, channel_num, sps, bps, no ) ) goto End;

	b_ret = true;
End:
	if( p_noise ) pxtone_Noise_Release( p_noise );

	return b_ret;
}

bool SEFF_Initialize( void )
{
	bool b_ret      = false;

	bool b_exterior = false;

	_noise.bOn = FALSE;
	_noise.no  = 0;

	if( !_Voice_Load( "cancel"  , SeNo_Cancel , b_exterior ) ) goto End;
	if( !_Voice_Load( "error"   , SeNo_Error  , b_exterior ) ) goto End;
	if( !_Voice_Load( "equip"   , SeNo_Equip  , b_exterior ) ) goto End;
	if( !_Voice_Load( "goal"    , SeNo_Goal   , b_exterior ) ) goto End;
	if( !_Voice_Load( "inquiry" , SeNo_Inquiry, b_exterior ) ) goto End;
	if( !_Voice_Load( "ok"      , SeNo_Ok     , b_exterior ) ) goto End;
	if( !_Voice_Load( "scream1" , SeNo_Scream1, b_exterior ) ) goto End;
	if( !_Voice_Load( "scream2" , SeNo_Scream2, b_exterior ) ) goto End;
	if( !_Voice_Load( "select"  , SeNo_Select , b_exterior ) ) goto End;
	if( !_Voice_Load( "walk"    , SeNo_Walk   , b_exterior ) ) goto End;

	b_ret = true;
End:

	return b_ret;
}

void SEFF_SetHear( short x, short y )
{
	_hear_x = x;
	_hear_y = y;
}

void SEFF_Release( void )
{
	if( _noise.bOn )
	{
		DxSound_Voice_Stop( _noise.no );
		_noise.bOn = FALSE;
		_noise.no  = 0;
	}
}


void SEFF_Voice_Release( long no )
{
	DxSound_Voice_Release( no );
}

void SEFF_Voice_Play( long no )
{
	if( !no && _bMute ) return;
	DxSound_Voice_Play( no );
}

void SEFF_Voice_Play( long no, short x, short y )
{
	if( !no && _bMute ) return;
	if( x < _hear_x - 1 ||
		x > _hear_x + 1 ||
		y < _hear_y - 1 ||
		y > _hear_y + 1 ) return;

	DxSound_Voice_Play( no );
}

void SEFF_Voice_Stop( long no )
{
	DxSound_Voice_Stop( no );
}


// noise
void SEFF_Noise_ON( long no )
{
	DxSound_Voice_Loop( no );
	_noise.bOn = TRUE;
	_noise.no  = no;
}

void SEFF_Noise_OFF( long no )
{
	DxSound_Voice_Stop( no );
	_noise.bOn = FALSE;
	_noise.no  = 0;
}

void SEFF_Noise_Inactive( void )
{
	if( _noise.bOn ) DxSound_Voice_Stop( _noise.no );
}

void SEFF_Noise_Active( void )
{
	if( _noise.bOn ) DxSound_Voice_Loop( _noise.no );
}

void SEFF_SetMute( BOOL b )
{
	_bMute = b;
}