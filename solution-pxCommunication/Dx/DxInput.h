#define MAX_JOYPADBUTTON	32

typedef struct{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	unsigned long button_bits;
}DXJOYPADSTRUCT;

BOOL DxInput_Initialize( HINSTANCE hInst, HWND hWnd );
void DxInput_Release(    void );
BOOL DxInput_Get( DXJOYPADSTRUCT *p_joypad );
BOOL DxInput_SetNeutral( void );
