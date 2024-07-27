#include <StdAfx.h>

#include "../Dx/DxDraw.h"
#include "../Fixture/MessageBox.h"
#include "../DataTable/dt_Stage.h"

#include "interface/Interface.h"
#include "interface/SurfaceNo.h"

extern char      g_module_path[];
extern dt_Stage  *g_stage;
extern HINSTANCE g_hInst;

bool StageProject_Load2( HWND hDlg, const char *name )
{
	bool b_ret = false;

	int err_flags = g_stage->Stage_Load( name );

	if( err_flags & ERRFLAG_ATTRIBUTE       ) mbox_ERR( hDlg, "Err: Load Attribute"          );
	if( err_flags & ERRFLAG_MAPPARTSIMAGE   ) mbox_ERR( hDlg, "Err: Load Map-Parts-Image"    );
	if( err_flags & ERRFLAG_MAP             ) mbox_ERR( hDlg, "Err: Load Map-Binary"         );
	if( err_flags & ERRFLAG_UNIT            ) mbox_ERR( hDlg, "Err: Load Unit-Binary"        );
	if( 0&&err_flags & ERRFLAG_lCASTIMAGE   ) mbox_ERR( hDlg, "Err: Load Unit-lCast-Image"   );
	if( 0&&err_flags & ERRFLAG_lSYMBOLIMAGE ) mbox_ERR( hDlg, "Err: Load Unit-lSymbol-Image" );

	Interface_Process( TRUE );

	return b_ret;
}

bool Stage_Project_Save2()
{
	return g_stage->Stage_Save();
}

//void MainWindow_SetTitle( const char *path );

/*
BOOL CALLBACK dlg_NewStage  ( HWND hDlg, UINT msg, WPARAM w, LPARAM l );
BOOL CALLBACK dlg_List_Stage( HWND hDlg, UINT msg, WPARAM w, LPARAM l );

void StageProject_Save( HWND hWnd )
{
	g_stage->Stage_Save();
}

void StageProject_New( HWND hWnd )
{
	if( DialogBox( g_hInst, "DLG_NEWSTAGE", hWnd, dlg_NewStage ) )
	{
		MainWindow_SetTitle( g_stage->Stage_GetName() );
	}
}

void StageProject_Load( HWND hWnd )
{
	if( DialogBox( g_hInst, "DLG_LIST_STAGE", hWnd, dlg_List_Stage ) )
	{
		MainWindow_SetTitle( g_stage->Stage_GetName() );
	}
}
*/