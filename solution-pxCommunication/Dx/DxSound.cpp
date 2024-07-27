#include <StdAfx.h>

#include "../DirectX5/dsound.h"
#include "../Fixture/DataDevice.h"

#define MAX_VOICE 64

static LPDIRECTSOUND       _p_DirectSound = NULL;            // DirectSoundオブジェクト
static BOOL                _bExterior     = FALSE;           // 外部DirectSoundフラグ
static LPDIRECTSOUNDBUFFER _p_secondarys[ MAX_VOICE ] = { NULL };   // 二時バッファ


////////////////////////////
// ローカル関数 ////////////
////////////////////////////

static BOOL _malloc_zero( void **pp, long size )
{
	*pp = malloc( size ); if( !( *pp ) ) return FALSE;
	memset( *pp, 0, size );              return TRUE;
}

static void _free( void **pp )
{
	if( *pp ){ free( *pp ); *pp = NULL; }
}


// PCMロード
#define _PCMHEADSIZE 44 // RIFF + WAVE + fmt_ + size x 3 + WAVEFORMATEX(20)

static BOOL _Load_PCM(
	const char    *file_name,
	const char    *type_name,
	WAVEFORMATEX  *p_format,
	long          *p_size,
	unsigned char **pp_data )
{
	BOOL bReturn = FALSE;
	long size;
	char buf[_PCMHEADSIZE];
	DDV  read;

	*pp_data = NULL;
	memset( p_format, 0, sizeof(WAVEFORMATEX)      );

	if( !ddv_Open( NULL, file_name, type_name, &read ) ) goto End;

	if( !ddv_Read( buf, 1, _PCMHEADSIZE, &read ) ) goto End;

	// 'RIFFxxxxWAVEfmt '
	if( buf[ 0] != 'R' || buf[ 1] != 'I' || buf[ 2] != 'F' || buf[ 3] != 'F' ||
		buf[ 8] != 'W' || buf[ 9] != 'A' || buf[10] != 'V' || buf[11] != 'E' ||
		buf[12] != 'f' || buf[13] != 'm' || buf[14] != 't' || buf[15] != ' ' ) goto End;

	// フォーマットを読み込む
	memcpy( p_format, &buf[20], 18 );

	if( p_format->wFormatTag     != WAVE_FORMAT_PCM                      ) goto End;
	if( p_format->nChannels      != 1 && p_format->nChannels      !=  2  ) goto End;
	if( p_format->wBitsPerSample != 8 && p_format->wBitsPerSample != 16  ) goto End;


	// 'fmt ' 以降に 'data' を探す
	ddv_Seek( &read, 12, SEEK_SET );
	while( ddv_Read( buf, 1, 8, &read ) ){
		memcpy( &size,       &buf[ 4 ], 4 );
		if( !memcmp( "data", &buf[ 0 ], 4 ) ){
			if( !_malloc_zero(    (void**)pp_data, size                               ) ) goto End;
			if( !ddv_Read( (void*)*pp_data, sizeof(unsigned char), size, &read ) ) goto End;
			*p_size = size;
			bReturn = TRUE;
			break;
		}
		ddv_Seek( &read, size, SEEK_CUR );
	}

End:
	if( !bReturn ) _free( (void**)pp_data );
	ddv_Close( &read );

	return bReturn;
}

////////////////////////////
// グローバル関数 //////////
////////////////////////////

void DxSound_SetDirectSoundPointer( void *p )
{
	_p_DirectSound = (LPDIRECTSOUND)p;
	_bExterior     = TRUE;
}

// DirectSoundの開始 
BOOL DxSound_Initialize( HWND hWnd )
{
	int i;

	if( DirectSoundCreate(NULL, &_p_DirectSound, NULL) != DS_OK ){
		_p_DirectSound = NULL;
		return FALSE;
	}
	_bExterior = FALSE;

	if( _p_DirectSound->SetCooperativeLevel( hWnd, DSSCL_PRIORITY ) != DS_OK ) return FALSE;

	for( i = 0; i < MAX_VOICE; i++ ) _p_secondarys[i] = NULL;

	return(TRUE);
}

// DirectSoundの終了 
void DxSound_Release(void)
{
	int i;

	if( !_p_DirectSound )return;
	for(i = 0; i < MAX_VOICE; i++){
		if(_p_secondarys[i] ){
			_p_secondarys[i]->Release();
			_p_secondarys[i] = NULL;
		}
	}
	if( !_bExterior && _p_DirectSound ) _p_DirectSound->Release();
}

// サウンドの読み込み(リソースから)
BOOL DxSound_Voice_Load_PCM( const char *file_name, const char *type_name, int no )
{
	BOOL          bReturn = FALSE;
	WAVEFORMATEX  fmt;
	DSBUFFERDESC  dsbd;
	unsigned char *p_data = NULL;
	long          data_size;

	if( !_p_DirectSound           ) return TRUE;
	if( no < 0 || no >= MAX_VOICE ) goto End;

	if( !_Load_PCM( file_name, type_name, &fmt, &data_size, &p_data ) ) goto End;

	// 二次バッファの初期化
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_GLOBALFOCUS|DSBCAPS_STATIC|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY; 
	dsbd.dwBufferBytes = data_size;
	dsbd.lpwfxFormat   = &fmt; 
	if( _p_DirectSound->CreateSoundBuffer(&dsbd, &_p_secondarys[no], NULL) != DS_OK) goto End;

	// 二次バッファのロック
	LPVOID lpbuf1, lpbuf2;
	DWORD  dwbuf1, dwbuf2;
	if( _p_secondarys[no]->Lock(0, data_size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0) != DS_OK) goto End;

	// 音源データの設定
	CopyMemory( lpbuf1, p_data, dwbuf1 );
	if( dwbuf2 ) CopyMemory(lpbuf2, p_data + dwbuf1, dwbuf2 );
	// 二次バッファのロック解除
	_p_secondarys[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 

	bReturn = TRUE;
End:
	_free( (void**)&p_data );
	if( !bReturn ){
		if( _p_secondarys[no] ){
			_p_secondarys[no]->Release();
			_p_secondarys[no] = NULL;
		}
	}
	return(TRUE);
}

// サウンドの作成
BOOL DxSound_Voice_Create(
	unsigned char* p_data   ,
	int            data_size,
	long           channel_num,
	long           sps,
	long           bps,
	int            no )
{
	BOOL          bReturn = FALSE;
	WAVEFORMATEX  fmt;
	DSBUFFERDESC  dsbd;

	if( !_p_DirectSound           ) return TRUE;
	if( no < 0 || no >= MAX_VOICE ) goto End;

//	if( !_Load_Noise( file_name, type_name, &fmt, &data_size, &p_data, channel_num, sps, bps ) ) goto End;

	memset( &fmt, 0, sizeof(fmt) );

	fmt.cbSize          = 0;
	fmt.wFormatTag      = WAVE_FORMAT_PCM  ;
	fmt.nChannels       = (WORD)channel_num;
	fmt.nSamplesPerSec  = sps              ;
	fmt.wBitsPerSample  = (WORD)bps        ;
	fmt.nBlockAlign     = fmt.nChannels * fmt.wBitsPerSample / 8;
	fmt.nAvgBytesPerSec = fmt.nBlockAlign * fmt.nSamplesPerSec;

	// 二次バッファの初期化
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_GLOBALFOCUS|DSBCAPS_STATIC|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY; 
	dsbd.dwBufferBytes = data_size;
	dsbd.lpwfxFormat   = &fmt; 
	if( _p_DirectSound->CreateSoundBuffer(&dsbd, &_p_secondarys[no], NULL) != DS_OK) goto End;

	// 二次バッファのロック
	LPVOID lpbuf1, lpbuf2;
	DWORD  dwbuf1, dwbuf2;
	if( _p_secondarys[no]->Lock(0, data_size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0) != DS_OK) goto End;

	// 音源データの設定
	CopyMemory( lpbuf1, p_data, dwbuf1 );
	if( dwbuf2 ) CopyMemory(lpbuf2, p_data + dwbuf1, dwbuf2 );
	// 二次バッファのロック解除
	_p_secondarys[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 

	bReturn = TRUE;
End:
//	_free( (void**)&p_data );
	if( !bReturn )
	{
		if( _p_secondarys[no] )
		{
			_p_secondarys[no]->Release();
			_p_secondarys[no] = NULL;
		}
	}
	return bReturn;
}




void DxSound_Voice_Release( int no )
{
	if( !_p_DirectSound           ) return;
    if( !_p_secondarys[no]        ) return;
	if( no < 0 || no >= MAX_VOICE ) return;

	_p_secondarys[no]->Release();
	_p_secondarys[no] = NULL;
}

// サウンドの再生 
void DxSound_Voice_Play( int no )
{
	if( !_p_DirectSound           ) return;
    if( !_p_secondarys[no]        ) return;
	if( no < 0 || no >= MAX_VOICE ) return;

	_p_secondarys[no]->Stop();
	_p_secondarys[no]->SetCurrentPosition(0);
    _p_secondarys[no]->Play(0, 0, 0);
}

void DxSound_Voice_Loop( int no )
{
	if( !_p_DirectSound           ) return;
    if( !_p_secondarys[no]        ) return;
	if( no < 0 || no >= MAX_VOICE ) return;

	_p_secondarys[no]->Play(0, 0, DSBPLAY_LOOPING);
}

void DxSound_Voice_Stop( int no )
{
	if( !_p_DirectSound           ) return;
    if( !_p_secondarys[no]        ) return;
	if( no < 0 || no >= MAX_VOICE ) return;

	_p_secondarys[no]->Stop();
}

