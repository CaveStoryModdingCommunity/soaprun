
long BGM_Stop(     long fade_msec );
void BGM_Inactive( void );
void BGM_Active(   void );
bool BGM_Play( int bgm_no );
int  BGM_GetNo();

enum
{
	BGM_NONE = 0,
	BGM_Soap,
	BGM_Ending,
	BGM_Kousin,
	BGM_Suiteki,
	BGM_RainDrop,
};
