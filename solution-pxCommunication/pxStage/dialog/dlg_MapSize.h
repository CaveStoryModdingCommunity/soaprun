typedef struct
{
	const char *p_title;
	int w;
	int h;
	int align_flags;

}MAPSIZESTRUCT;

BOOL CALLBACK dlg_MapSize( HWND hDlg, UINT msg, WPARAM w, LPARAM l );
