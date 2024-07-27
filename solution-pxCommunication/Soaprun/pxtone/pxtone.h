#ifdef PXTONEDLL_EXPORTS
#define DLLAPI __declspec(dllexport) // DLL ����鑤�� export �Ƃ���
#else
#define DLLAPI __declspec(dllimport) // DLL ���g������ import �Ƃ���
#endif

// C++ �ł� C �ł��g����悤�ɂ���
#ifdef __cplusplus
extern "C"{
#endif



typedef BOOL (* PXTONEPLAY_CALLBACK)( long clock );

// �ȉ� pxtone�֐��Q ==========================================

// pxtone �𐶐����܂��B
BOOL DLLAPI pxtone_Ready(
	HWND hWnd,          // �E�C���h�E�n���h����n���Ă�������
	long channel_num,   // �̃`�����l�������w�肵�Ă��������B( 1:���m���� / 2:�X�e���I )
	long sps,           // �b�ԃT���v�����O���[�g�ł��B      ( 11025 / 22050 / 44100 )
	long bps,           // �P�T���v����\������r�b�g���ł��B( 8 / 16 )
	float buffer_sec,   // �Ȃ��Đ�����̂Ɏg�p����o�b�t�@�T�C�Y��b�Ŏw�肵�܂��B( ���� 0.1 )
	BOOL bDirectSound,  // TRUE: DirectSound ���g�p���܂� / FALSE: WAVEMAPPER ���g�p���܂��B
	PXTONEPLAY_CALLBACK pProc // �T���v�����O���ɌĂ΂��֐��ł��BNULL �ł��܂��܂���B
); // pxtone �̏���

// pxtone ���Đݒ肵�܂��B
BOOL DLLAPI pxtone_Reset(
	HWND hWnd,
	long channel_num,
	long sps,
	long bps,
	float buffer_sec,
	BOOL bDirectSound,
	PXTONEPLAY_CALLBACK pProc
);

// pxtone �Ő������ꂽDirectSound�̃|�C���^(LPDIRECTSOUND)���擾����B
// �擾����DirectSound�͎����Ń����[�X���Ȃ��悤�ɒ��ӂ��Ă��������B
void DLLAPI *pxtone_GetDirectSound( void );

// ���X�g�G���[������擾
const char DLLAPI* pxtone_GetLastError( void );

// pxtone �̉������擾���܂�
void DLLAPI pxtone_GetQuality( long *p_channel_num, long *p_sps, long *p_bps, long *p_sample_per_buf ); // pxtone

// pxtone ���J�����܂�
BOOL DLLAPI pxtone_Release( void );

// �Ȃ�ǂݍ��݂܂�(�t�@�C���E���\�[�X����)
BOOL DLLAPI pxtone_Tune_Load(
	HMODULE hModule,       // ���\�[�X����ǂޏꍇ�̓��W���[���n���h�����w�肵�܂��BNULL �ł����Ȃ������B
	const char *type_name, // ���\�[�X����ǂޏꍇ�̓��\�[�X�̎�ޖ��B�O���t�@�C����ǂޏꍇ�� NULL�B
	const char *file_name  // �t�@�C���p�X�������̓��\�[�X���B
	);

// �Ȃ�ǂݍ���(����������)
BOOL DLLAPI pxtone_Tune_Read( void* p, long size );

// �Ȃ�������܂�
BOOL DLLAPI pxtone_Tune_Release( void );

// �Ȃ��Đ����܂�
BOOL DLLAPI pxtone_Tune_Start(
	long start_sample,     // �J�n�ʒu�ł��B��� Stop �� Fadeout �Ŏ擾�����l��ݒ肵�܂��B0 �ōŏ�����B
	long fadein_msec       // �t�F�[�h�C������ꍇ�͂����Ɏ��ԁi�~���b�j���w�肵�܂��B
	);

// �t�F�[�h�A�E�g�X�C�b�`�����Č��ݍĐ��T���v�����擾���܂�
long DLLAPI pxtone_Tune_Fadeout( long msec );

// �Ȃ̃{�����[����ݒ肵�܂��B1.0 ���ő�ŁA0.5 �������ł��B
void DLLAPI pxtone_Tune_SetVolume( float v );

// �Ȃ��~���Č��ݍĐ��T���v�����擾
long DLLAPI pxtone_Tune_Stop( void );

// �Đ������ǂ����𒲂ׂ܂�
BOOL DLLAPI pxtone_Tune_IsStreaming( void );

// ���[�v�Đ��� ON/OFF ��؂�ւ��܂�
void DLLAPI pxtone_Tune_SetLoop( BOOL bLoop );

// �Ȃ̏����擾���܂�
void DLLAPI pxtone_Tune_GetInformation( long *p_beat_num, float *p_beat_tempo, long *p_beat_clock, long *p_meas_num );

// ���s�[�g���߂��擾���܂�
long DLLAPI pxtone_Tune_GetRepeatMeas( void );

// �L�����t���߂��擾���܂�(LAST�C�x���g���߁B������΍ŏI����)
long DLLAPI pxtone_Tune_GetPlayMeas(   void );

// �Ȃ̖��̂��擾���܂�
const char DLLAPI* pxtone_Tune_GetName(    void );

// �Ȃ̃R�����g���擾���܂�
const char DLLAPI* pxtone_Tune_GetComment( void );

// ���w��̃A�h���X�ɍĐ��o�b�t�@���������݂܂�
BOOL DLLAPI pxtone_Tune_Vomit(
	void* p,         // �Đ��o�b�t�@��f���o���A�h���X�ł�
	long sample_num  // �������ރT���v�����ł�
	);
// �߂�l�F�܂�����������ꍇ�� TRUE ����ȊO�� FALSE
// �P�A���̊֐����g�p����ꍇ�� pxtone_Ready() �̈��� buffer_sec �ɂ� 0 ��ݒ肵�Ă��������B
//     pxtone �̃X�g���[�~���O�@�\�������ɂȂ�܂��B
// �Q�A�Ȃ̃��[�h�� pxtone_Tune_Start() ���I���Ă��炱�̊֐����Ăяo���Ă��������B
// �R�Asample_num �̓T�C�Y�ł͂Ȃ��ăT���v�����ł��B
//     ��) 11025hz 2ch 8bit ���P�b�f���o���ꍇ�Asample_num �� 11025 ���w�肷��B
//         p �ɂ� 22050�o�C�g���̍Đ��o�b�t�@���������܂��B
// �S�A�X�g���[�~���O�@�\���L���Ȏ��Ɠ��l�� pxtone_Tune_Fadeout() ���̊֐����g���܂��B




// �s�X�g���m�C�Y�𐶐����܂�
typedef struct
{
	unsigned char* p_buf;
	int            size ;

}PXTONENOISEBUFFER;

void              DLLAPI  pxtone_Noise_Release( PXTONENOISEBUFFER* p_noise );
PXTONENOISEBUFFER DLLAPI *pxtone_Noise_Create(
	const char* name   ,     // ���\�[�X��      ��ݒ�B�O���t�@�C���̏ꍇ�̓t�@�C���p�X�B
	const char* type   ,     // ���\�[�X�^�C�v����ݒ�B�O���t�@�C���̏ꍇ��NULL�B
	long        channel_num, long sps, long bps );

#ifdef __cplusplus
}
#endif
