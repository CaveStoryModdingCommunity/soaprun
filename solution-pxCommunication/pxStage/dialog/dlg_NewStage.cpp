#include <StdAfx.h>

#include "../../Fixture/WindowRect.h"
#include "../../Fixture/MessageBox.h"

#include "../../DataTable/dt_Stage.h"

#include "../resource.h"

extern dt_Stage* g_stage;

static char* _rect_name = "newstage.rect";

bool _Apply_NewStage( HWND hDlg )
{
	char stage_name[ BUFFERSIZE_STAGENAME ];

	GetDlgItemText( hDlg, IDC_STAGE_NAME, stage_name, BUFFERSIZE_STAGENAME );

	if( !g_stage->Stage_New( stage_name ) )
	{
		mbox_ERR( hDlg, "can't new stage: %s", stage_name );
		return false;
	}
	return true;
}

BOOL CALLBACK dlg_NewStage( HWND hDlg, UINT msg, WPARAM w, LPARAM l )
{
	// IME off
	if( msg == WM_COMMAND && HIWORD( w ) == EN_SETFOCUS )
	{
		switch( LOWORD( w ) )
		{
		case IDC_STAGE_NAME   :
			HIMC hImc = ImmGetContext( hDlg );
            ImmSetOpenStatus( hImc, FALSE );
            ImmReleaseContext( hDlg, hImc );
			break;
		}
	}

	switch( msg )
	{
	case WM_INITDIALOG:

		if( !WindowRect_Load( hDlg, _rect_name, TRUE ) ) WindowRect_Centering( hDlg );

		break;

	case WM_CLOSE:
		WindowRect_Save( hDlg, _rect_name );
		EndDialog( hDlg, TRUE );
		break;

	case WM_COMMAND:

		switch( LOWORD(w) )
		{
		case IDOK:
			if( !_Apply_NewStage( hDlg ) ) break;
			EndDialog( hDlg, TRUE  );
			break;

		case IDCANCEL:
			EndDialog( hDlg, FALSE );
			break;
		}
		break;

	default: return FALSE;
	}
	return TRUE;
}