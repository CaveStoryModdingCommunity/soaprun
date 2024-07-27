#pragma once

#define _CGI

#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <stdarg.h>
#include <math.h>

#include <shlwapi.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32")
#pragma comment(lib, "shlwapi")

#include <shlobj.h>

#include <process.h>

#include <imm.h>
#pragma comment(lib, "imm32")

#pragma comment(lib, "winmm")

extern HINSTANCE g_hInst       ;
extern char      *g_dir_data   ;
extern char      *g_dir_profile;
extern HANDLE    g_hEve_Exit   ;

#define FIELDVIEW_W    320
#define FIELDVIEW_H    240
#define WINDOW_W       480
#define WINDOW_H       240

#define VS 512

enum POSFROM
{
	POSFROM_LT = 0,
	POSFROM_RT  ,
	POSFROM_LB  ,
	POSFROM_RB  ,
	POSFROM_HC_T,
	POSFROM_HC_B,
	POSFROM_HO_T,
	POSFROM_HO_B,
};

#define UNITCOLOR_NUM 4
#define DRAWCOLOR_NUM 4

enum
{
	SfNo_LOADING = 0,
	SfNo_FONT_1     ,
	SfNo_FRAME_B    ,
	SfNo_FRAME_H    ,
	SfNo_FRAME_M    ,
	SfNo_FRAME_V    ,
	SfNo_KEYCODE    ,
	SfNo_COLOR_BGFIELD,
	SfNo_COLOR_BGTEXT ,
	SfNo_COLOR_TEXT   ,
	SfNo_TITLE        ,

	SfNo_MapParts     ,
	SfNo_fuPlayer     ,
	SfNo_fuNPU        ,
	SfNo_fuVenger     ,
	SfNo_Shadow       ,
	SfNo_Equip        ,
	SfNo_Weather      ,
	SfNo_EndSnaps     ,
};

#define DIALOGUE_PADDING_X       1
#define DIALOGUE_PADDING_Y       0
#define DIALOGUE_SCROLL_SPEED    4
#define DIALOGUE_LISTINDENT_W   10

#define MPARTSATTNUM_H  16
#define MPARTSATTNUM_V  16
#define MPARTSGRIDNUM_H 16
#define MAPPARTS_W      16
#define MAPPARTS_H      16

#define MAPPARTS_hfW     8
#define MAPPARTS_hfH     8



#define BUFSIZE_MAPNAME 32

