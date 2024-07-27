#include <pthread.h>  // pthread_xxx()

class svListener
{
private:

	enum _LISTENERSTATUS
	{
		_LISTENER_IDLE = 0  ,
		_LISTENER_ACTIVE    ,
		_LISTENER_ERR_THREAD,
		_LISTENER_ERR_LISTEN,
		_LISTENER_ERR_ACCEPT,
	};

	_LISTENERSTATUS _status ;
	int             _sock   ;
	pthread_t       _thrd   ;
	int             _port_no;

public :

	svListener ( int port_no );
	~svListener();

	bool BindAndListen();
	bool Accept       ( int *p_sock_cl );

	bool Kill();
	bool WaitExit();
};
