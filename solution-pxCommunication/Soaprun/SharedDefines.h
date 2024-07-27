#pragma once

#define BUFFERSIZE_IPADDRESS    64
#define BUFFERSIZE_HOSTID       16

#define BUFFERSIZE_ATTENDER  10000         // Attender が送受信で使用
#define BUFFERSIZE_TEST        512         // 通信テストで使用

#define BUFFERSIZE_REQSTRING    64
#define BUFFERSIZE_PROTOCOL      8
#define BUFFERSIZE_REQNAME       4         // リクエスト名の長さ
#define BUFFERSIZE_RESNAME       4         // リクエスト名の長さ

#define MAX_FIELDSIZE_W        320
#define MAX_FIELDSIZE_L        240

#define MAX_CLIENTSOCKET        96
#define MAX_ENABLESOCKET        64         // MAX:256

#define MAX_OTHERUNIT           (MAX_ENABLESOCKET-1)
#define MAX_NPU                 64         // MAX:256

#define FLDUNITPOSHISTORY_NUM    8

#define ROOMGRIDNUM_H           20
#define ROOMGRIDNUM_V           15

// connection protocol.
extern int        g_port_no         ;
extern const char *g_protocol_name  ;
extern short      g_protocol_version;

// Ring Buffers.
#define BUFFERSIZE_RING_MSG 10240       // 画面表示メッセージ 
#define BUFFERSIZE_RING_REQ  1024       // サーバーへのリクエスト
#define BUFFERSIZE_RING_RES (1024*1024) // サーバーからの応答

enum clREQUEST
{
	clREQUEST_None        = 0,
	clREQUEST_Protocol       ,
	clREQUEST_MapAttribute   ,
	clREQUEST_Room           ,
	clREQUEST_MyPosition     ,
	clREQUEST_mCorpse        ,
	clREQUEST_Test           ,
	clREQUEST_Dlog           ,
	clREQUEST_Bye            ,
	clREQUEST_INQ_HitNPU     ,
	clREQUEST_INQ_HitVenger  ,
	clREQUEST_INQ_ChangeColor,
	clREQUEST_INQ_DrawFloor  ,

	clREQUEST_num
};

enum svRESPONSE
{
	svRESPONSE_None     = 0,
	svRESPONSE_Protocol    ,
	svRESPONSE_MapAttribute,
	svRESPONSE_Room        ,
	svRESPONSE_Field       ,
	svRESPONSE_Test        ,
	svRESPONSE_Void        ,

	svRESPONSE_num
};

extern const char *g_req_names[];
extern const char *g_res_names[];
extern const char *g_npu_names[];

enum NPUTYPE
{
	NPU_Goal   ,
	NPU_Closer ,
	NPU_Sword  ,
	NPU_Crawl  ,
	NPU_Hummer ,

	NPU_Rounder,
	NPU_Wuss   ,
	NPU_Chase  ,
	NPU_Gate   ,
	NPU_Shield ,

	NPU_Cross  ,
	NPU_Snail  ,
};

enum fuSTAT
{
	fuSTAT_Suspend = 0,
	fuSTAT_Idle       ,
	fuSTAT_Dead       ,
	fuSTAT_Goal       ,
	fuSTAT_Ghost      ,
	fuSTAT_Disable 
};

#define EQUIPFLAG_SWORD    0x01
#define EQUIPFLAG_CROWN    0x02
#define EQUIPFLAG_SHIELD   0x04

#define mPartAttr_None     0x00
#define mPartAttr_Block    0x01
#define mPartAttr_bNPU     0x02
#define mPartAttr_bPlayer  0x03

enum WEATHER
{
	WEATHER_None = 0,
	WEATHER_Rain,
};
