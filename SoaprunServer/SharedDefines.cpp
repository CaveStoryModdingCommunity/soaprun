#include "SharedDefines.h"

int        g_port_no          =      1001;

const char *g_protocol_name   = "Soaprun";
short      g_protocol_version =        64;

const char *g_req_names[ clREQUEST_num ] =
{
	"-"   , //None        = 0
	"Prtc", //Protocol       
	"mAtt", //MapAttribute   
	"Room", //Room           
	"myPo", //MyPosition     
	"mCrp", //mCorpse        
	"Test", //Test           
	"Dlog", //Dlog           
	"Bye.", //Bye            
	"HNPU",  //INQ_HitNPU     
	"HVen", //INQ_HitVenger  
	"ChCl", //INQ_ChangeColor
	"DrFl"  //INQ_DrawFloor  
};

const char *g_res_names[ svRESPONSE_num ] =
{
	"-"   , //None     = 0
	"Prtc", //Protocol    
	"mAtt", //MapAttribute
	"Room", //Room        
	"Flds", //Field       
	"Test", //Test        
	"Void"  //Void        
};

const char *g_npu_names[] =
{
	"Goal   ",
	"Closer ",
	"Sword  ",
	"Crawl  ",
	"Hummer ",
			
	"Rounder",
	"Wuss   ",
	"Chase  ",
	"Gate   ",
	"Shield ",
			
	"Cross  ",
	"Snail  ",
};
