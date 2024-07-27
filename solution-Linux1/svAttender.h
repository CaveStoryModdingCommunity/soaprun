#include <pthread.h>  // pthread_xxx()

//void *svAttender_Thread( void *arg );

class svAttender
{
private:

	enum _ATTENDERSTATUS
	{
		_ATTENDER_IDLE = 0,
		_ATTENDER_ACTIVE  ,
		_ATTENDER_ERROR   ,
	};

	_ATTENDERSTATUS _status;
	int             _sock  ;
	pthread_t       _thrd  ;
	int             _no    ;

public :

	svAttender( int no );
	~svAttender();
	int No();
	bool WaitExit();
};
