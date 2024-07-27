#include "SharedDefines.h"


// Request ----
extern const char *g_req_names[];

typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; short version;    }reqPROTOCOL    ;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ];                   }reqMAPATTRIBUTE;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; char  x; char  y; }reqROOM        ;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; int str_len;      }reqDLOG        ;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ];                   }reqBYE         ;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; short x; short y; }reqMAPCORPSE   ;

typedef struct{ char req_name[ BUFFERSIZE_REQNAME ];                                            char pos_his_num; }reqMYPOSITION     ;
																		  
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; unsigned char uc_tgt;                      char pos_his_num; }reqINQ_HitNPU     ;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ];                                            char pos_his_num; }reqINQ_HitVenger  ;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; char param1;                               char pos_his_num; }reqINQ_ChangeColor;
typedef struct{ char req_name[ BUFFERSIZE_REQNAME ]; short fld_x; short fld_y; unsigned char i; char pos_his_num; }reqINQ_DrawFloor  ;

// Response ----
extern const char *g_res_names[];

typedef struct{ char res_name[ BUFFERSIZE_RESNAME ]; char protocol[ BUFFERSIZE_PROTOCOL ]; unsigned short version; }resPROTOCOL;
typedef struct{ char res_name[ BUFFERSIZE_RESNAME ]; short w; short l; }resMAPATTRIBUTE;
typedef struct{ char res_name[ BUFFERSIZE_RESNAME ]; char  x; char  y; }resROOM        ;
typedef struct{ char res_name[ BUFFERSIZE_RESNAME ];                   }resVOID;

typedef struct
{
	char          res_name[ BUFFERSIZE_RESNAME ];

	char          own_status ;
	char          own_param1 ;
	char          own_param2 ;

	unsigned char num_other  ;
	unsigned char num_npu    ;
	unsigned char num_mparts ;
	unsigned char weather    ;
}
resFIELD_HEAD;

typedef struct
{
	unsigned char index      ;
	unsigned char gene_count ;
	char          status     ;
	char          param1     ;
	char          param2     ;
	char          pos_his_num;
}
resFIELD_fUNIT;

typedef struct
{
	short         x    ;
	short         y    ;
	unsigned char parts;
}
resFIELD_MPARTS;

void Request_Dlog   ( const char *fmt, ... );
void Request_mCorpse( short x, short y );
void Request_Bye    ();
