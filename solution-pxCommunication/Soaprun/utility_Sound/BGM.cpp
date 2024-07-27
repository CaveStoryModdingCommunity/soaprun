#include <StdAfx.h>

#include "../pxtone/pxtone.h"

static const char *_ext     = "ptcop";
static long _inactive_clock = 0;
static BOOL _bPlay          = FALSE;
static int  _bgm_no         = 0;

#define _INACTIVEFADE 200

typedef struct
{
	const char* name       ;
	long        start_clock;
}_BGMTABLE;

static _BGMTABLE _bgms[] =
{
	{ ""            , 0 },
	{ "soap"        , 0 },
	{ "yukinowatari", 0 },
	{ "kousin"      , 0 },
	{ "suiteki"     , 0 },
	{ "RainDrop"    , 0 },
};

bool BGM_Play( int bgm_no )
{
//	return true;

	char path[ MAX_PATH ];



	if( !bgm_no )
	{
		_inactive_clock = pxtone_Tune_Fadeout( _INACTIVEFADE );
		_bPlay = FALSE;
		return true;
	}
	if( _bPlay && bgm_no == _bgm_no ) return true;

	pxtone_Tune_Stop();
	
	sprintf( path, "%s\\%s.%s", g_dir_data, _bgms[ bgm_no ].name, _ext );
	if( !pxtone_Tune_Load( NULL, NULL, path ) ) return false;
	if( !pxtone_Tune_Start( _bgms[ bgm_no ].start_clock, 0 ) ) return false;
	_bPlay  = TRUE  ;
	_bgm_no = bgm_no;
	return true;
}

long BGM_Stop( long fade_msec )
{
	_bPlay = FALSE;
	return pxtone_Tune_Fadeout( fade_msec );
}

void BGM_Inactive( void )
{
	if( _bPlay )
	{
		_inactive_clock = pxtone_Tune_Fadeout( _INACTIVEFADE );
	}
}

void BGM_Active( void )
{
	if( _bPlay )
	{
		pxtone_Tune_Start( _inactive_clock, _INACTIVEFADE );
	}
}

int BGM_GetNo( void )
{
	return _bgm_no;
}
