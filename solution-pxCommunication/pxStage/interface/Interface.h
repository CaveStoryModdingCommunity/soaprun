// RECTÉTÉCÉY

#define FIELDGRID      16

#define VIEWDEFAULT_W 160
#define VIEWDEFAULT_H 120

//#define DOORPARTS_NUM_H 16
//#define DOORPARTS_NUM_V 16


#define UNITPARTS_NUM_H 16
#define UNITPARTS_NUM_V 16

//#define ATTRIBUTE_W (MAPPARTS_GRID * ATTRIBUTE_NUM_H)
//#define ATTRIBUTE_H (MAPPARTS_GRID * ATTRIBUTE_NUM_V)

extern RECT g_rcView;



void Interface_Process(    BOOL bDraw );
BOOL Interface_Initialize( HWND hWnd );
void Interface_Release();
