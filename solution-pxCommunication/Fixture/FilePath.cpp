#include "StdAfx.h"

//extern char* g_dir_temp;

BOOL FilePath_SelectLoad( HWND hWnd, char *path, const char *title, const char *ext, const char *filter )
{
	OPENFILENAME ofn;//ファイルネーム構造体
	char         start_path[ MAX_PATH ];
	memset( &ofn, 0, sizeof(OPENFILENAME) );//ゼロクリア

	strcpy( start_path, path );
	PathRemoveFileSpec( start_path );
	memset( path, 0, MAX_PATH );

	ofn.lStructSize     = sizeof(OPENFILENAME);//構造体のサイズ
	ofn.hwndOwner       = hWnd;//親ウインドウハンドル
	ofn.lpstrFilter     = filter;
//		"text files {*.txt}\0*.txt\0"\
//		"All files {*.*}\0*.*\0\0";
	ofn.lpstrFile       = path;//ファイルパスを格納するポインタ及び初期パス
	ofn.nMaxFile        = MAX_PATH;//パスサイズ
	ofn.lpstrFileTitle  = NULL;//選択ファイル名を入れるポインタ
	ofn.nMaxFileTitle   = 0;//選択ファイル名サイズ
	ofn.lpstrInitialDir = start_path;//初期ディレクトリ（NULLでカレントディレクトリになる）
	ofn.lpstrTitle      = title;//ダイアログのキャプション
	ofn.Flags           = NULL
//				|OFN_ALLOWMULTISELECT	//複数ファイル選択可（\0で区切られる）●
//				|OFN_CREATEPROMPT		//無いファイルの場合、許可を求める●
				|OFN_FILEMUSTEXIST		//既存のファイルのみ許可●
				|OFN_HIDEREADONLY		//読み取り専用チェックボックスを隠す
//				|OFN_NOCHANGEDIR		//カレントディレクトリを移動しない
//				|OFN_NOLONGNAMES		//長いファイル名を表示しない
				|OFN_PATHMUSTEXIST		//既存のパスのみ許可●
				;
	ofn.lpstrDefExt     = ext;//デフォルトでつけられる拡張子

	if( GetOpenFileName( &ofn ) )//←これがメイン
		return TRUE;
	return FALSE;
}

//セーブファイルコモンダイアログを呼ぶ
BOOL FilePath_SelectSave( HWND hWnd, char *path, const char *title, const char *ext, const char *filter )
{
	OPENFILENAME ofn;//ファイルネーム構造体
	memset( &ofn, 0, sizeof(OPENFILENAME) );//ゼロクリア

	ofn.lStructSize     = sizeof(OPENFILENAME);//構造体のサイズ
	ofn.hwndOwner       = hWnd;//親ウインドウハンドル
	ofn.lpstrFilter     = filter;
//		"text {*.txt}\0*.txt\0"\
//		"All files {*.*}\0*.*\0\0";
	ofn.lpstrFile       = path;//ファイルパスを格納するポインタ及び初期パス
	ofn.nMaxFile        = MAX_PATH;//パスサイズ
	ofn.lpstrFileTitle  = NULL;//選択ファイル名を入れるポインタ
	ofn.nMaxFileTitle   = 0;//選択ファイル名サイズ
	ofn.lpstrInitialDir = NULL;//初期ディレクトリ（NULLでカレントディレクトリになる）
	ofn.lpstrTitle      = title;//ダイアログのキャプション
	ofn.Flags           = NULL
				|OFN_HIDEREADONLY		//読み取り専用チェックボックスを隠す
//				|OFN_NOCHANGEDIR		//カレントディレクトリを移動しない
//				|OFN_NOLONGNAMES		//長いファイル名を表示しない
				|OFN_OVERWRITEPROMPT	//既存のファイルの場合、許可を求める●
				;
	ofn.lpstrDefExt     = ext;//デフォルトでつけられる拡張子

	if( GetSaveFileName( &ofn ) )//←これがメイン
		return TRUE;
	return FALSE;
}


/*
// 最終フォルダを読み込む
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

// 最終アクセスフォルダを書き込む
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
// ファイルパスに使用できない文字を x に変換する
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
// フォルダー選択
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

	//　これをしないとインターフェイスはダメなのです。
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pPath, -1, ochPath, MAX_PATH);

	//　実際にITEMIDLISTを取得します。
	hRes = pDesktopFolder->ParseDisplayName( NULL, NULL, ochPath, &chEaten, &pIDL, &dwAttributes);
    
	if(hRes != NOERROR)
		pIDL = NULL;

	pDesktopFolder->Release();

	return pIDL;
}

static int CALLBACK _BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	//　初期化時にフォルダを選択させます。
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

	//　pidlRetFolderにはそのフォルダを表すアイテムＩＤリストへのポインタが
	//　入っています。chPutFolderには選択されたフォルダ名（非フルパス）だけ
	//　しか入っていないので、このポインタを利用します。
	if(pidlRetFolder != NULL) {
		//　フルパスを取得します。
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
