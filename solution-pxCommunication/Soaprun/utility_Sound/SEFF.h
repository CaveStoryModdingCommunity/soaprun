
enum
{
	SeNo_None   = 0,
	SeNo_Cancel    ,
	SeNo_Error     ,
	SeNo_Equip     ,
	SeNo_Goal      ,
	SeNo_Inquiry   ,
	SeNo_Ok        ,
	SeNo_Scream1   ,
	SeNo_Scream2   ,
	SeNo_Select    ,
	SeNo_Walk      ,
	SeNo_num
};

bool SEFF_Initialize( void );
void SEFF_Release(    void );
BOOL SEFF_Voice_Load( const char *name, long no, BOOL bExterior );
void SEFF_Voice_Play(     long no );
void SEFF_Voice_Play(     const char* name );
void SEFF_Voice_Play( long no, short x, short y );
void SEFF_SetHear   (          short x, short y );

void SEFF_Voice_Stop(     long no );
void SEFF_Voice_Release(  long no );
void SEFF_Noise_ON(       long no );
void SEFF_Noise_OFF(      long no );
void SEFF_Noise_Inactive( void    );
void SEFF_Noise_Active(   void    );
void SEFF_SetMute(        BOOL b  );

extern const char* g_souv_names[];

