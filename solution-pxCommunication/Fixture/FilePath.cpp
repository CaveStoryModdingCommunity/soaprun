#include "StdAfx.h"

//extern char* g_dir_temp;

BOOL FilePath_SelectLoad( HWND hWnd, char *path, const char *title, const char *ext, const char *filter )
{
	OPENFILENAME ofn;//�t�@�C���l�[���\����
	char         start_path[ MAX_PATH ];
	memset( &ofn, 0, sizeof(OPENFILENAME) );//�[���N���A

	strcpy( start_path, path );
	PathRemoveFileSpec( start_path );
	memset( path, 0, MAX_PATH );

	ofn.lStructSize     = sizeof(OPENFILENAME);//�\���̂̃T�C�Y
	ofn.hwndOwner       = hWnd;//�e�E�C���h�E�n���h��
	ofn.lpstrFilter     = filter;
//		"text files {*.txt}\0*.txt\0"\
//		"All files {*.*}\0*.*\0\0";
	ofn.lpstrFile       = path;//�t�@�C���p�X���i�[����|�C���^�y�я����p�X
	ofn.nMaxFile        = MAX_PATH;//�p�X�T�C�Y
	ofn.lpstrFileTitle  = NULL;//�I���t�@�C����������|�C���^
	ofn.nMaxFileTitle   = 0;//�I���t�@�C�����T�C�Y
	ofn.lpstrInitialDir = start_path;//�����f�B���N�g���iNULL�ŃJ�����g�f�B���N�g���ɂȂ�j
	ofn.lpstrTitle      = title;//�_�C�A���O�̃L���v�V����
	ofn.Flags           = NULL
//				|OFN_ALLOWMULTISELECT	//�����t�@�C���I���i\0�ŋ�؂���j��
//				|OFN_CREATEPROMPT		//�����t�@�C���̏ꍇ�A�������߂遜
				|OFN_FILEMUSTEXIST		//�����̃t�@�C���̂݋���
				|OFN_HIDEREADONLY		//�ǂݎ���p�`�F�b�N�{�b�N�X���B��
//				|OFN_NOCHANGEDIR		//�J�����g�f�B���N�g�����ړ����Ȃ�
//				|OFN_NOLONGNAMES		//�����t�@�C������\�����Ȃ�
				|OFN_PATHMUSTEXIST		//�����̃p�X�̂݋���
				;
	ofn.lpstrDefExt     = ext;//�f�t�H���g�ł�����g���q

	if( GetOpenFileName( &ofn ) )//�����ꂪ���C��
		return TRUE;
	return FALSE;
}

//�Z�[�u�t�@�C���R�����_�C�A���O���Ă�
BOOL FilePath_SelectSave( HWND hWnd, char *path, const char *title, const char *ext, const char *filter )
{
	OPENFILENAME ofn;//�t�@�C���l�[���\����
	memset( &ofn, 0, sizeof(OPENFILENAME) );//�[���N���A

	ofn.lStructSize     = sizeof(OPENFILENAME);//�\���̂̃T�C�Y
	ofn.hwndOwner       = hWnd;//�e�E�C���h�E�n���h��
	ofn.lpstrFilter     = filter;
//		"text {*.txt}\0*.txt\0"\
//		"All files {*.*}\0*.*\0\0";
	ofn.lpstrFile       = path;//�t�@�C���p�X���i�[����|�C���^�y�я����p�X
	ofn.nMaxFile        = MAX_PATH;//�p�X�T�C�Y
	ofn.lpstrFileTitle  = NULL;//�I���t�@�C����������|�C���^
	ofn.nMaxFileTitle   = 0;//�I���t�@�C�����T�C�Y
	ofn.lpstrInitialDir = NULL;//�����f�B���N�g���iNULL�ŃJ�����g�f�B���N�g���ɂȂ�j
	ofn.lpstrTitle      = title;//�_�C�A���O�̃L���v�V����
	ofn.Flags           = NULL
				|OFN_HIDEREADONLY		//�ǂݎ���p�`�F�b�N�{�b�N�X���B��
//				|OFN_NOCHANGEDIR		//�J�����g�f�B���N�g�����ړ����Ȃ�
//				|OFN_NOLONGNAMES		//�����t�@�C������\�����Ȃ�
				|OFN_OVERWRITEPROMPT	//�����̃t�@�C���̏ꍇ�A�������߂遜
				;
	ofn.lpstrDefExt     = ext;//�f�t�H���g�ł�����g���q

	if( GetSaveFileName( &ofn ) )//�����ꂪ���C��
		return TRUE;
	return FALSE;
}


/*
// �ŏI�t�H���_��ǂݍ���
BOOL FilePath_LoadLastFolder( char *last_path, const char *name )
{
	FILE *fp;
	char path[MAX_PATH];

	sprintf( path, "%s\\%s", g_dir_temp, name );
	fp = fopen( path, "rb" );
	if( !fp ) return FALSE;

	if( fread( last_path, sizeof(char), MAX_PATH, fp ) != MAX_PATH )
	{
		fclose( fp );
		return FALSE;
	}
	fclose( fp );
	if( !PathIsDirectory( last_path ) ) return FALSE;

	return TRUE;
}

// �ŏI�A�N�Z�X�t�H���_����������
BOOL FilePath_SaveLastFolder( const char *last_path, const char *name )
{
	FILE *fp;
	char path[MAX_PATH];

	sprintf( path, "%s\\%s", g_dir_temp, name );
	fp = fopen( path, "wb" );
	if( !fp ) return FALSE;

	strcpy( path, last_path );
	PathRemoveFileSpec( path );
	if( fwrite( path, sizeof(char), MAX_PATH, fp ) != MAX_PATH )
	{
		fclose( fp );
		return FALSE;
	}
	fclose( fp );
	return TRUE;
}
*/
// �t�@�C���p�X�Ɏg�p�ł��Ȃ������� x �ɕϊ�����
void FilePath_Fatal_x( char *name )
{
	long i = 0;
	while( name[ i ] != '\0' ){
		if( name[ i ] == '\\' ||
			name[ i ] == '/'  ||
			name[ i ] == ':'  ||
			name[ i ] == ','  ||
			name[ i ] == ';'  ||
			name[ i ] == '*'  ||
			name[ i ] == '?'  ||
			name[ i ] == '\"' ||
			name[ i ] == '<'  ||
			name[ i ] == '>'  ||
			name[ i ] == '|'  ) name[ i ] = 'x';

		i++;
	}
}






//////////////////
// �t�H���_�[�I��
//////////////////
#include <windows.h>
#include <Shlobj.h>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")

static LPITEMIDLIST _GetItemIDList( char *pPath )
{
	LPITEMIDLIST pIDL;
	LPSHELLFOLDER pDesktopFolder; 

	if(::SHGetDesktopFolder(&pDesktopFolder) != NOERROR)
		return NULL;

	OLECHAR ochPath[MAX_PATH];
	ULONG chEaten;
	ULONG dwAttributes;
	HRESULT hRes;

	//�@��������Ȃ��ƃC���^�[�t�F�C�X�̓_���Ȃ̂ł��B
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pPath, -1, ochPath, MAX_PATH);

	//�@���ۂ�ITEMIDLIST���擾���܂��B
	hRes = pDesktopFolder->ParseDisplayName( NULL, NULL, ochPath, &chEaten, &pIDL, &dwAttributes);
    
	if(hRes != NOERROR)
		pIDL = NULL;

	pDesktopFolder->Release();

	return pIDL;
}

static int CALLBACK _BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	//�@���������Ƀt�H���_��I�������܂��B
	if(uMsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, FALSE, lpData);
	} else if(uMsg == BFFM_SELCHANGED) {
		char chText[MAX_PATH];
		if(SHGetPathFromIDList((LPITEMIDLIST)lParam, chText))
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, TRUE, (LPARAM)chText);
	}

	return 0;
}

BOOL FilePath_SelectDirectory( HWND hWnd, const char *title, char *path )
{
	BOOL bRet = FALSE;
	BOOL bRes;

	char chPutFolder[MAX_PATH];
	LPITEMIDLIST pidlRetFolder = 0;
	LPITEMIDLIST pFirstIDL     = 0;
	BROWSEINFO stBInfo;

	if( !path ) return FALSE;

	pFirstIDL         = _GetItemIDList( path );

	memset(&stBInfo, 0, sizeof(BROWSEINFO));
	stBInfo.hwndOwner = hWnd;
	stBInfo.lpszTitle = title;
	stBInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
	stBInfo.lpfn      = _BrowseCallbackProc;
	stBInfo.lParam    = (LPARAM)pFirstIDL;
		

	pidlRetFolder = SHBrowseForFolder(&stBInfo);

	//�@pidlRetFolder�ɂ͂��̃t�H���_��\���A�C�e���h�c���X�g�ւ̃|�C���^��
	//�@�����Ă��܂��BchPutFolder�ɂ͑I�����ꂽ�t�H���_���i��t���p�X�j����
	//�@���������Ă��Ȃ��̂ŁA���̃|�C���^�𗘗p���܂��B
	if(pidlRetFolder != NULL) {
		//�@�t���p�X���擾���܂��B
		bRes = SHGetPathFromIDList(pidlRetFolder, chPutFolder);
		if(bRes != FALSE) {
			strcpy( path, chPutFolder);
			bRet = TRUE;
		}

		CoTaskMemFree(pidlRetFolder);
	}

	if( pFirstIDL ) CoTaskMemFree( pFirstIDL );

	return bRet;
}
