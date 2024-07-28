#ifdef NDEBUG

#include <windows.h>

#else

#include <pthread.h>  // pthread_xxx()

#endif

#define MAX_REQUESTVALU_NUM   8



class svAttender
{
private:

	enum _ATTENDERSTATUS
	{
		_ATTENDER_IDLE = 0,
		_ATTENDER_ACTIVE  ,
		_ATTENDER_ERROR   ,
	};

#ifdef NDEBUG

	HANDLE          _thrd        ;

#else

	pthread_t       _thrd        ;

#endif

	bool            _b_ok        ;
	bool            _b_stop      ;
	bool            _b_over_quota;
	_ATTENDERSTATUS _status      ;
	const LINKUNIT  *_p_link     ;
	int             _index       ;
	char            *_buf        ;
	int             _buf_size    ;
	char            *_mbuf       ;
	int             _mbuf_size   ;
	dt_MapData      *_room_send  ;
	unsigned char   _gene_count  ;

	unsigned int    _last_update_id;

	int  _Response_Protocol    ();
	int  _Response_MapAttribute();
	int  _Response_Room        ( char x, char y );
	int  _Response_Test        ();
//	int  _Response_Dlog        ();
//	int  _Response_Bye         ();
	int  _Response_FieldUnits  ();
//	int  _Response_MapCorpse   ();
//	int  _Response_Inquiry     ( bool b_result );
//	int  _Response_Inquiry( char type, char count );
	int  _Response_Void        ();


public :

	svAttender( int index, bool b_over_quota );
	~svAttender();
	int  Index();

	void LinkUnit_Set  ( const LINKUNIT *p_link_unit );
	void LinkUnit_Close();

	bool WaitExit();

	bool RecvRequiestAndSendResponse();
	int  SendQuota();
};
