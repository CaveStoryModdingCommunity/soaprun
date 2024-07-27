#pragma once

#define KEY_BUTTON_1     0x01
#define KEY_BUTTON_2     0x02
#define KEY_BUTTON_3     0x03
#define KEY_BUTTON_D     0x04
#define KEY_BUTTON_LEFT  0x10
#define KEY_BUTTON_RIGHT 0x20
#define KEY_BUTTON_UP    0x40
#define KEY_BUTTON_DOWN  0x80

enum enum_Buttons_dir{

	enum_Buttons_dir_None = 0,
	enum_Buttons_dir_Left,
	enum_Buttons_dir_Right,
	enum_Buttons_dir_Up,
	enum_Buttons_dir_Down,
	enum_Buttons_dir_LU,
	enum_Buttons_dir_RU,
	enum_Buttons_dir_LD,
	enum_Buttons_dir_RD,
};

enum enum_Buttons_KeyOn{

	enum_Buttons_KeyOn_Left ,
	enum_Buttons_KeyOn_Right,
	enum_Buttons_KeyOn_Up   ,
	enum_Buttons_KeyOn_Down ,
	enum_Buttons_KeyOn_Action,
	enum_Buttons_KeyOn_Cancel,
	enum_Buttons_KeyOn_NUM  ,
};

enum enum_Buttons_JoyOn{

	enum_Buttons_JoyOn_Shot ,
	enum_Buttons_JoyOn_Pause,
	enum_Buttons_JoyOn_NUM  ,
};


typedef struct{

	unsigned char key_codes[ enum_Buttons_KeyOn_NUM ];
	unsigned char joy_codes[ enum_Buttons_JoyOn_NUM ];

}BUTTONSCONFIGSTRUCT;

typedef struct{

	long btn;
	long trg;
	long old;

}BUTTONSSTRUCT;

void Buttons_GetConfigDefault(          BUTTONSCONFIGSTRUCT *p_conf );
void Buttons_GetConfigDefault_Keyboard( BUTTONSCONFIGSTRUCT *p_conf );
void Buttons_GetConfigDefault_Joypad(   BUTTONSCONFIGSTRUCT *p_conf );
BOOL Buttons_SaveConfig( const BUTTONSCONFIGSTRUCT *p_conf );
BOOL Buttons_LoadConfig(       BUTTONSCONFIGSTRUCT *p_conf );

void Buttons_ReloadConfig(   void );
long Buttons_GetButtons(     void );
bool Buttons_IsJoypad(       void );

void Buttons_Reset(          BUTTONSSTRUCT *p_btns );
void Buttons_UpdateTriggers( BUTTONSSTRUCT *p_btns );

enum_Buttons_dir Buttons_GetDirection( long btn );

