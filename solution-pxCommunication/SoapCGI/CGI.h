#pragma once

#include "ServerInformation.h"

//#include "NameID.h"
/*
enum CGIf
{
	CGIf_None = 0,
	CGIf_Hello,
	CGIf_Text,
	CGIf_Void,
	CGIf_Bye ,

	CGIf_PlanetPositionValue,
	CGIf_MissileDirection   ,

	CGIf_Feel,

	CGIf_num ,
};


//extern const char * g_cgif_names[];


enum SESSIONPHASE
{
	SESSION_INVALID = 0,

	SESSION_Lobby      ,
	SESSION_GetID      ,
	SESSION_Waiting    ,
	SESSION_Calling    ,

	SESSION_Table      ,
	SESSION_None       ,
};

typedef struct
{
	char id  [ BUFFERSIZE_ID   ];
	char name[ BUFFERSIZE_NAME ];
}
PLAYERSTRUCT;


typedef struct
{
	PLAYERSTRUCT own  ;
	PLAYERSTRUCT other;
	SESSIONPHASE phase;
	int          msg_count;
	bool         b_server ;

	char         err_msg[ BUFFERSIZE_ERR ];
}
SESSIONSTRUCT;
*/

void CGI_Initialize( void );
void CGI_Release   ( void );
//bool CGI_GetServerInformation(       SERVERINFORMATION *p_sv );
bool CGI_GetServerInformation(       SERVERINFORMATION *p_sv, const bool *p_b_exit );

bool CGI_SetServerInformation( const SERVERINFORMATION *p_sv, const bool *p_b_exit );

//bool CGI_GetUserList( NAMEIDSTRUCT *entries , int max_entry      );
