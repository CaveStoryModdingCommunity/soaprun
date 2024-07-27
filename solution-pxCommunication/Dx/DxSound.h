
BOOL DxSound_Initialize( HWND hWnd ); // DirectSound 初期化
void DxSound_Release( void );         // DirectSound 開放
void DxSound_Voice_Release( int no ); // 音 開放
void DxSound_Voice_Play(    int no ); // 音 再生
void DxSound_Voice_Loop(    int no ); // 音 ループ
void DxSound_Voice_Stop(    int no ); // 音 停止

BOOL DxSound_Voice_Load_PCM( const char *file_name, const char *type_name, int no );
/*
BOOL DxSound_Voice_Load_Noise(
	const char    *file_name,
	const char    *type_name,
	long          channel_num,
	long          sps,
	long          bps,
	int           no );
*/
BOOL DxSound_Voice_Create(
	unsigned char* p_data   ,
	int            data_size,
	long           channel_num,
	long           sps,
	long           bps,
	int            no );

// pxtone 用
void DxSound_SetDirectSoundPointer( void *p );
