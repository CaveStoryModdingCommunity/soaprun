#include <StdAfx.h>

#include "../DirectX5/dinput.h"
#include "DxInput.h"

LPDIRECTINPUT			_pDInput = NULL;
LPDIRECTINPUTDEVICE2    _pDevice = NULL;


// ジョイスティック検索用
typedef struct {
	LPDIRECTINPUT			pDInput;					// DirectInputオブジェクトのポインタ
	LPDIRECTINPUTDEVICE2	pDevice;
} DIRECTINPUTDEVICEINFO;

static _neutral_x = 0;
static _neutral_y = 0;


// ジョイスティックの列挙
static BOOL CALLBACK DIEnumCallback( LPCDIDEVICEINSTANCE lpDDI, LPVOID lpContext )
{
	HRESULT					     dirval;
	static LPDIRECTINPUTDEVICE	 pDev1 ;
	static LPDIRECTINPUTDEVICE2	 pDev2 ;
	static DIRECTINPUTDEVICEINFO *p_info = (DIRECTINPUTDEVICEINFO*)lpContext ;

	// 一時的にデバイスを生成
	if( p_info->pDInput->CreateDevice( lpDDI->guidInstance, &pDev1, NULL ) != DI_OK ){
		p_info->pDevice = NULL;
		return DIENUM_CONTINUE;
	}

	// DIRECTINPUTDEVICE ではジョイスティックを使えないので DIRECTINPUTDEVICE2 を使う
	// COM では QueryInterfaceに対して直接比較を行わず、FAILEDマクロを使うようにする
	dirval = pDev1->QueryInterface( IID_IDirectInputDevice2, (void **) &pDev2 ) ;
	if( FAILED( dirval ) ){ _pDevice = NULL; return DIENUM_CONTINUE; }
	if( pDev1 ){ pDev1->Release(); pDev1 = NULL; }
	p_info->pDevice = pDev2 ;

	return DIENUM_STOP;
}


// アクティブ状態
static BOOL _Activate( BOOL bActive )
{
	if( !_pDevice ) return TRUE;
	if( bActive ) _pDevice->Acquire(  );
	else          _pDevice->Unacquire();
	return TRUE;
}

// ジョイスティックデバイスの初期化
static BOOL initDeviceJoystick( HWND hWnd )
{
	DIRECTINPUTDEVICEINFO info;

	info.pDevice = NULL;
	info.pDInput = _pDInput;
	info.pDInput->AddRef();

	_pDInput->EnumDevices( DIDEVTYPE_JOYSTICK, DIEnumCallback, &info, DIEDFL_ATTACHEDONLY ) ;// Old?
	if( info.pDInput  ){ info.pDInput->Release(); info.pDInput = NULL; }
	if( !info.pDevice ) return FALSE ;
	_pDevice = info.pDevice ;

	// データフォーマット設定
	if( DI_OK != _pDevice->SetDataFormat( &c_dfDIJoystick ) ) return FALSE;
	if( DI_OK != _pDevice->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND ) ) return FALSE;

	// アクセス権の取得
	_pDevice->Acquire() ;

	return TRUE ;
}

////////////////////
// 以下グローバル関数
////////////////////

// 初期化
BOOL DxInput_Initialize( HINSTANCE hInst, HWND hWnd )
{
	if( DI_OK != DirectInputCreate( hInst, DIRECTINPUT_VERSION, &_pDInput, NULL ) ) return FALSE;
	if( !initDeviceJoystick( hWnd ) ) return FALSE;
	return TRUE ;
}

// 開放
void DxInput_Release( void )
{
	if( _pDevice ){ _pDevice->Release(); _pDevice = NULL; }
	if( _pDInput ){ _pDInput->Release(); _pDInput = NULL; }
}

BOOL DxInput_Get( DXJOYPADSTRUCT *p_joypad )
{
	DIJOYSTATE state;

	if( !_pDevice                 ) return FALSE;
	if( _pDevice->Poll() != DI_OK ) return FALSE; // デバイスデータが更新される

	// 状態の取得
	switch( _pDevice->GetDeviceState( sizeof(DIJOYSTATE), &state ) ){
	case DI_OK          :                     break;
	case DIERR_INPUTLOST: _Activate( FALSE ); break;
	default             : return FALSE;
	}

	// ボタン
	for( long b = 0; b < MAX_JOYPADBUTTON; b++ ){
		if( state.rgbButtons[ b ] & 0x80 ) p_joypad->button_bits |=  ( 0x01 << b );
		else                               p_joypad->button_bits &= ~( 0x01 << b );
	}

	p_joypad->bLeft = p_joypad->bUp = p_joypad->bRight = p_joypad->bDown = FALSE;

	if(      state.lX < _neutral_x - 10000 ) p_joypad->bLeft  = TRUE;
	else if( state.lX > _neutral_x + 10000 ) p_joypad->bRight = TRUE;
	if(      state.lY < _neutral_y - 10000 ) p_joypad->bUp    = TRUE;
	else if( state.lY > _neutral_y + 10000 ) p_joypad->bDown  = TRUE;

	return TRUE ;
}

BOOL DxInput_SetNeutral( void )
{
	DIJOYSTATE		state ;

	if( !_pDevice                 ) return FALSE ;
	if( _pDevice->Poll() != DI_OK ) return FALSE ; // デバイスデータが更新される

	// 状態の取得
	switch( _pDevice->GetDeviceState( sizeof(DIJOYSTATE), &state ) ){
	case DI_OK          :                     break;
	case DIERR_INPUTLOST: _Activate( FALSE ); break;
	default             : return FALSE;
	}

	_neutral_x = state.lX;
	_neutral_y = state.lY;

	return TRUE ;
}
