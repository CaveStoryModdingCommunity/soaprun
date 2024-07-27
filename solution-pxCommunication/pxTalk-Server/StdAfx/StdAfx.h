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


#define BUFFERSIZE_ERR       100

#define BUFFERSIZE_NAME       32
#define BUFFERSIZE_ID         12
#define BUFFERSIZE_TEXT      160
#define BUFFERSIZE_TALKVIEW 1024

#define BUFFERSIZE_RECEIVE  1024


extern HINSTANCE g_hInst       ;
extern char      *g_dir_data   ;
extern char      *g_dir_profile;


extern const char *g_app_name;
