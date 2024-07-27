#include <StdAfx.h>

#include "../Dx/DxDraw.h"

#pragma comment(lib,"version")

#include "utility_Draw/ListFrame.h"
#include "utility_Draw/PxImage.h"
#include "utility_Sound/SEFF.h"
#include "utility_Sound/BGM.h"

#include "scene_Buttons.h"
#include "Scene.h"
#include "Online.h"


//#include "Triangle.h"
//#include "Weather.h"
//static Weather *_wt = NULL;
//extern Random         g_ran       ;

#define _CONTENTS_NUM 2

extern DWORD      g_color_bg_text;
extern RECT       g_rc_wnd   ;
extern Scene      *g_scene   ;
extern int        g_version  ;
//extern Triangle   *g_tri     ;

static ListFrame  *_lf_mode  ;
static ListFrame  *_lf_ver   ;
static SIMPLELIST _list_ver  ;
static char       _ver_str   [ 10 ];
static SIMPLELIST _lists_mode[ _CONTENTS_NUM ];


static int _GetCompileVersion( long *p1, long *p2, long *p3, long *p4 )
{
	LPVOID            p = NULL;
	DWORD             dummy;
	DWORD             size ;
	VS_FIXEDFILEINFO* info ;
	UINT              vSize; 
	char              path[ MAX_PATH ];

	long v[ 4 ];

	memset( v, 0, sizeof(long) * 4 );
	if( p1 ) *p1 = 0;
	if( p2 ) *p2 = 0;
	if( p3 ) *p3 = 0;
	if( p4 ) *p4 = 0;

	GetModuleFileName( NULL, path, MAX_PATH );

	size = GetFileVersionInfoSize( path, &dummy ); 
	if( !size ) goto End;

	p = malloc( size ); 
	if( !p                                                ) goto End;
	if( !GetFileVersionInfo( path, 0, size, p )           ) goto End;
	if( !VerQueryValue( p, "\\", (LPVOID*)&info, &vSize ) ) goto End;

	v[ 0 ] = HIWORD(info->dwFileVersionMS);
	v[ 1 ] = LOWORD(info->dwFileVersionMS);
	v[ 2 ] = HIWORD(info->dwFileVersionLS);
	v[ 3 ] = LOWORD(info->dwFileVersionLS);

	if( p1 ) *p1 = v[ 0 ];
	if( p2 ) *p2 = v[ 1 ];
	if( p3 ) *p3 = v[ 2 ];
	if( p4 ) *p4 = v[ 3 ];

End:
	if( p ) free( p );

	return v[ 0 ] * 1000 + v[ 1 ] * 100 + v[ 2 ] * 10 + v[ 3 ];
}

void scene_Title_Initialize( void )
{
	SIMPLELIST lists_mode[ _CONTENTS_NUM ] =
	{
		{ SCENE_Connection, "スタート！"    , NULL, false },
		{ SCENE_Buttons   , "ボタンせってい", NULL, false },
	};
	memcpy( _lists_mode, lists_mode, sizeof(SIMPLELIST) * _CONTENTS_NUM );

	long                              v1,  v2,  v3,  v4  ;
	g_version = _GetCompileVersion(  &v1, &v2, &v3, &v4 );
	sprintf( _ver_str, "v.%d.%d%d%d", v1,  v2,  v3,  v4 );
	_list_ver.p_str    = _ver_str;
	_list_ver.p_buf    = NULL    ;

	// バージョン
	_lf_ver = new ListFrame( LISTFRAME_STATIC, 6, 1, POSFROM_HC_T, 8, 48, false );
	_lf_ver->List_Set( &_list_ver  , 1 );

	// メニュー
	_lf_mode = new ListFrame( LISTFRAME_LIST , 8, _CONTENTS_NUM, POSFROM_HC_T, 8, 70, false );
	_lf_mode->List_Set( _lists_mode, _CONTENTS_NUM );

	PxImage_Load( "title", SfNo_TITLE, FALSE  );


//	_wt = new Weather( FIELDVIEW_W, FIELDVIEW_H, 12, &g_ran );
}

void scene_Title_Release( void )
{
	if( _lf_mode ) delete _lf_mode;
	if( _lf_ver  ) delete _lf_ver ;
}

void scene_Title_Reset( void )
{
	BGM_Stop( 500 );
	_lf_mode->List_Set_CursorY( 0 );
}


void scene_Title_Procedure( const BUTTONSSTRUCT* p_btns )
{
	RECT rc_title = { 0,  0, 120, 32 };
	RECT rc_pixel = { 0, 32, 120, 40 };

//	RECT rc_test = {0,0,16,16};
//	static unsigned char deg = 0;

	if( Online_IsWaitEvent() && p_btns->trg & KEY_BUTTON_1 )
	{
		int ret = _lf_mode->List_Get_CursorParam();
		SEFF_Voice_Play( SeNo_Ok );

		switch( ret )
		{
		case SCENE_Title   :
			break;

		case SCENE_Buttons: scene_Buttons_Set(); break;
		}
		g_scene->Set( (SCENE)ret );
		return;
	}
//	if( p_btns->trg & KEY_BUTTON_2 )
//	{
//	}

	_lf_ver ->Action( p_btns );
	_lf_mode->Action( p_btns );

	// 表示
	DxDraw_PaintRect( &g_rc_wnd, g_color_bg_text );
	DxDraw_Put( ( WINDOW_W - (rc_title.right - rc_title.left) ) / 2,  16, &rc_title, SfNo_TITLE );
	DxDraw_Put( ( WINDOW_W - (rc_pixel.right - rc_pixel.left) ) / 2, g_rc_wnd.bottom - 16, &rc_pixel, SfNo_TITLE );


	const RECT* rc_view = &g_rc_wnd;

//	deg+=2;
//	DxDraw_Put( 100 + (g_tri->GetCos( (unsigned char)deg ) * 16 )/VS,
//				100 + (g_tri->GetSin( (unsigned char)deg ) * 16 )/VS, &rc_test, SfNo_fuNPU );

	_lf_ver ->Put( rc_view );
	if( Online_IsWaitEvent() ) _lf_mode->Put( rc_view );
}
