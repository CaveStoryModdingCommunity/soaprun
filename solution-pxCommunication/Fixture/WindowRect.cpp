#include "StdAfx.h"

static const char* _p_dir_data = NULL;

void WindowRect_Initialize( const char* p_dir_data )
{
	_p_dir_data = p_dir_data;
}

// �e�E�C���h�E�̐^���ɔz�u
void WindowRect_Centering( HWND hWnd )
{
	HWND hParent;
	RECT rcParent, rcMe, rcDesk;
	long x, y;

	// �f�X�N�g�b�v�N���C�A���g�̈���擾
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesk, 0 );
	GetWindowRect( hWnd, &rcMe );

	// �e��RECT���擾
	if( hParent = GetParent( hWnd ) )
	{
		WINDOWPLACEMENT place;
		place.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement( hParent, &place );
		switch( place.showCmd )
		{
		case SW_SHOWMINIMIZED:
		case SW_HIDE         :
			SystemParametersInfo( SPI_GETWORKAREA, 0, &rcParent, 0 );
			break;
		default:
			GetWindowRect( hParent, &rcParent );
		}
	}
	else
	{
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rcParent, 0 );
	}

	// �ʒu
	x = rcParent.left + ( (rcParent.right  - rcParent.left) - (rcMe.right  - rcMe.left) ) /2;
	y = rcParent.top  + ( (rcParent.bottom - rcParent.top ) - (rcMe.bottom - rcMe.top ) ) /2;

	// �X�N���[�����ɔ[�܂�悤�ړ�
	if( x                          < rcDesk.left   ) x = rcDesk.left;
	if( y                          < rcDesk.top    ) y = rcDesk.top;
	if( x + (rcMe.right-rcMe.left) > rcDesk.right  ) x = rcDesk.right  - (rcMe.right-rcMe.left);
	if( y + (rcMe.bottom-rcMe.top) > rcDesk.bottom ) y = rcDesk.bottom - (rcMe.bottom-rcMe.top);
	
	SetWindowPos( hWnd, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_DRAWFRAME );

	PostMessage( hWnd, WM_SIZE, SIZE_RESTORED, 0 );
}

// �E�C���h�ERECT�̃��[�h
// (WM_CREATE�ŌĂԂƃt���X�N���[���֐��Ŏ��s����)
BOOL WindowRect_Load( HWND hWnd, const char* name, BOOL bSize )
{
	FILE* fp;
	RECT  rect;
	char  path[MAX_PATH];

	RECT  rcDesk;
	long  max_w, max_h, min_w, min_h;
	long  showCmd = SW_NORMAL;

	if( !_p_dir_data ) return FALSE;

	sprintf( path, "%s\\%s", _p_dir_data, name );
	fp = fopen( path, "rb" );
	if( !fp ) return FALSE;

	fread( &rect,    sizeof(RECT), 1, fp );
	fread( &showCmd, sizeof(long), 1, fp );
	fclose( fp );

	// �f�X�N�g�b�v�N���C�A���g�̈���擾
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcDesk, 0 );
	// �E�C���h�E�ő�/�ŏ��T�C�Y���擾
	max_w = GetSystemMetrics( SM_CXMAXIMIZED );
	max_h = GetSystemMetrics( SM_CYMAXIMIZED );
	min_w = GetSystemMetrics( SM_CXMIN       );
	min_h = GetSystemMetrics( SM_CYMIN       );

	// ��������
	if( rect.right  - rect.left < min_w ) rect.right  = rect.left + min_w;
	if( rect.bottom - rect.top  < min_h ) rect.bottom = rect.top  + min_h;
	// �傫����
	if( rect.right  - rect.left > max_w ) rect.right  = rect.left + max_w;
	if( rect.bottom - rect.top  > max_h ) rect.bottom = rect.top  + max_w;

	// �X�N���[�����ɔ[�܂�p�Ɉړ�
	if( rect.left < rcDesk.left )
	{
		rect.right  += ( rcDesk.left - rect.left );
		rect.left   += ( rcDesk.left - rect.left );
	}
	if( rect.top  < rcDesk.top )
	{
		rect.bottom += ( rcDesk.top  - rect.top  );
		rect.top    += ( rcDesk.top  - rect.top  );
	}
	if( rect.right  > rcDesk.right )
	{
		rect.left   -= rect.right  - rcDesk.right;
		rect.right  -= rect.right  - rcDesk.right;
	}
	if( rect.bottom > rcDesk.bottom )
	{
		rect.top    -= rect.bottom - rcDesk.bottom;
		rect.bottom -= rect.bottom - rcDesk.bottom;
	}

	// �ړ��̂݁^�T�C�Y���w��
	if( bSize ) MoveWindow(   hWnd, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, FALSE );
	else        SetWindowPos( hWnd, NULL, rect.left, rect.top, 0,0, SWP_NOSIZE );

	if( showCmd == SW_SHOWMAXIMIZED ) ShowWindow( hWnd, SW_SHOWMAXIMIZED );
	else                              ShowWindow( hWnd, SW_NORMAL        );

	return TRUE;
}
 
// �E�C���h�ERECT�̃Z�[�u
BOOL WindowRect_Save( HWND hWnd, const char* name )
{
	FILE*           fp;
	WINDOWPLACEMENT place;
	char            path[MAX_PATH];
	RECT            rc;

	if( !_p_dir_data ) return FALSE;

	// �E�C���h�E�̏�Ԃ��擾����
	if( !GetWindowPlacement( hWnd, &place ) ) return FALSE;
	// �m�[�}���\���̏ꍇ�̓X�N���[�����W���擾
	if( place.showCmd == SW_NORMAL )
	{
		if( !GetWindowRect( hWnd, &rc ) ) return FALSE;
		place.rcNormalPosition = rc;
	}

	sprintf( path, "%s\\%s", _p_dir_data, name );
	fp = fopen( path, "wb" );
	if( !fp ) return FALSE;

	fwrite( &place.rcNormalPosition, sizeof(RECT), 1, fp );
	fwrite( &place.showCmd,          sizeof(long), 1, fp );
	fclose( fp );

	return TRUE;
}