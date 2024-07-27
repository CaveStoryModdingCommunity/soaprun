
enum SCENE
{
	SCENE_Reset =  0,
	SCENE_Exit      ,
	SCENE_Title     ,
	SCENE_Connection,
	SCENE_Buttons   ,
	SCENE_Game      ,
	SCENE_Ending    ,
	SCENE_GameOver  ,
};

class Scene
{
private:

	SCENE _scene     ;
	bool  _b_error   ;
	bool  _b_reset   ;
	char  _err_msg[ 260 ];

public :

	Scene( SCENE scene );
	void  Set( SCENE scene );
	SCENE Get();
	bool  IsError();
	void  SetError( const char* fmt, ... );
	void  Reset_Set   ();
	bool  Reset_Is    ();
	void  Reset_Remove();
};
