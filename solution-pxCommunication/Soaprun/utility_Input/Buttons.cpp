#include <StdAfx.h>

#include "../../Dx/DxInput.h"
#include "../../Fixture/KeyControl.h"

#include "Buttons.h"

static char *_conf_name    = "buttons.bin";

static BUTTONSCONFIGSTRUCT _key_config;
static bool                _b_keyboard;

void Buttons_GetConfigDefault_Keyboard( BUTTONSCONFIGSTRUCT *p_conf )
{
	p_conf->key_codes[ enum_Buttons_KeyOn_Left  ] = VK_LEFT   ;
	p_conf->key_codes[ enum_Buttons_KeyOn_Right ] = VK_RIGHT  ;
	p_conf->key_codes[ enum_Buttons_KeyOn_Up    ] = VK_UP     ;
	p_conf->key_codes[ enum_Buttons_KeyOn_Down  ] = VK_DOWN   ;
	p_conf->key_codes[ enum_Buttons_KeyOn_Action] = 'Z';//VK_DOWN   ;//VK_CONTROL;
	p_conf->key_codes[ enum_Buttons_KeyOn_Cancel] = 'X';//VK_TAB    ;
}

void Buttons_GetConfigDefault_Joypad(   BUTTONSCONFIGSTRUCT *p_conf )
{
	p_conf->joy_codes[ enum_Buttons_JoyOn_Shot  ] =          0;
	p_conf->joy_codes[ enum_Buttons_JoyOn_Pause ] =          1;
}

void Buttons_GetConfigDefault( BUTTONSCONFIGSTRUCT *p_conf )
{
	Buttons_GetConfigDefault_Keyboard( p_conf );
	Buttons_GetConfigDefault_Joypad(   p_conf );
}

// セーブ
BOOL Buttons_SaveConfig( const BUTTONSCONFIGSTRUCT *p_conf )
{
	BOOL bReturn = FALSE;
	FILE *fp = NULL;
	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s", g_dir_profile, _conf_name );
	fp = fopen( path, "wb" );
	if( !fp ) goto End;
	if( fwrite( p_conf, sizeof(BUTTONSCONFIGSTRUCT), 1, fp ) != 1 ) goto End;

	bReturn = TRUE;
End:
	if( fp ) fclose( fp );

	return bReturn;
}

// ロード
BOOL Buttons_LoadConfig( BUTTONSCONFIGSTRUCT *p_conf )
{
	BOOL bReturn = FALSE;
	FILE *fp     = NULL;
	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s", g_dir_profile, _conf_name );
	fp = fopen( path, "rb" );
	if( !fp ) goto End;
	if( fread( p_conf, sizeof(BUTTONSCONFIGSTRUCT), 1, fp ) != 1 ) goto End;

	bReturn = TRUE;
End:
	if( fp ) fclose( fp );

	return bReturn;
}

void Buttons_ReloadConfig( void )
{
	if( !Buttons_LoadConfig( &_key_config ) ) Buttons_GetConfigDefault( &_key_config );
}

long Buttons_GetButtons( void )
{
	long btn = 0;

	_b_keyboard = false;

	if( KeyControl_IsKey( _key_config.key_codes[ enum_Buttons_KeyOn_Left  ] ) ) btn  |= KEY_BUTTON_LEFT ;
	if( KeyControl_IsKey( _key_config.key_codes[ enum_Buttons_KeyOn_Right ] ) ) btn  |= KEY_BUTTON_RIGHT;
	if( KeyControl_IsKey( _key_config.key_codes[ enum_Buttons_KeyOn_Up    ] ) ) btn  |= KEY_BUTTON_UP   ;
	if( KeyControl_IsKey( _key_config.key_codes[ enum_Buttons_KeyOn_Down  ] ) ) btn  |= KEY_BUTTON_DOWN ;
	if( KeyControl_IsKey( _key_config.key_codes[ enum_Buttons_KeyOn_Action] ) )
		btn  |= KEY_BUTTON_1    ;
	if( KeyControl_IsKey( _key_config.key_codes[ enum_Buttons_KeyOn_Cancel] ) ) btn  |= KEY_BUTTON_2    ;

	if( btn ) _b_keyboard = true;

	DXJOYPADSTRUCT joypad;

	if( DxInput_Get( &joypad ) )
	{
		if( joypad.bLeft  ) btn |= KEY_BUTTON_LEFT ;
		if( joypad.bRight ) btn |= KEY_BUTTON_RIGHT;
		if( joypad.bUp    ) btn |= KEY_BUTTON_UP   ;
		if( joypad.bDown  ) btn |= KEY_BUTTON_DOWN ;
		if( joypad.button_bits & ( 1 << _key_config.joy_codes[ enum_Buttons_JoyOn_Shot  ] ) ) btn |= KEY_BUTTON_1;
		if( joypad.button_bits & ( 1 << _key_config.joy_codes[ enum_Buttons_JoyOn_Pause ] ) ) btn |= KEY_BUTTON_2;
	}
	
	return btn;
}

bool Buttons_IsJoypad( void )
{
	return _b_keyboard ? false : true;
}

void Buttons_Reset( BUTTONSSTRUCT *p_btns )
{
	KeyControl_Clear();
	p_btns->btn = Buttons_GetButtons();
	p_btns->old = p_btns->btn;
	p_btns->trg = 0;
}

void Buttons_UpdateTriggers( BUTTONSSTRUCT *p_btns )
{
	p_btns->trg = p_btns->btn ^ p_btns->old;
	p_btns->trg = p_btns->btn & p_btns->trg;
	p_btns->old = p_btns->btn;
}

enum_Buttons_dir Buttons_GetDirection( long btn )
{
	if(      btn & KEY_BUTTON_LEFT  && btn & KEY_BUTTON_UP    ) return enum_Buttons_dir_LU   ;
	else if( btn & KEY_BUTTON_RIGHT && btn & KEY_BUTTON_UP    ) return enum_Buttons_dir_RU   ;
	else if( btn & KEY_BUTTON_LEFT  && btn & KEY_BUTTON_DOWN  ) return enum_Buttons_dir_LD   ;
	else if( btn & KEY_BUTTON_RIGHT && btn & KEY_BUTTON_DOWN  ) return enum_Buttons_dir_RD   ;
	else if( btn & KEY_BUTTON_LEFT                            ) return enum_Buttons_dir_Left ;
	else if( btn & KEY_BUTTON_RIGHT                           ) return enum_Buttons_dir_Right;
	else if( btn & KEY_BUTTON_UP                              ) return enum_Buttons_dir_Up   ;
	else if( btn & KEY_BUTTON_DOWN                            ) return enum_Buttons_dir_Down ;

	return enum_Buttons_dir_None;
}

