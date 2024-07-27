
BOOL DxSound_Initialize( HWND hWnd ); // DirectSound ������
void DxSound_Release( void );         // DirectSound �J��
void DxSound_Voice_Release( int no ); // �� �J��
void DxSound_Voice_Play(    int no ); // �� �Đ�
void DxSound_Voice_Loop(    int no ); // �� ���[�v
void DxSound_Voice_Stop(    int no ); // �� ��~

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

// pxtone �p
void DxSound_SetDirectSoundPointer( void *p );
