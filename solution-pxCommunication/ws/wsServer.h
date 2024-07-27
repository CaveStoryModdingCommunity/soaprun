#include "ws.h"

class wsServer
{
private:

	bool       _b_valid      ;
//	func_STARTED  started_func ;
	char       _host_name[ MAX_HOSTNAME ];
	char       _host_ip  [ MAX_HOSTNAME ];
	HANDLE     _hEvent_Exit;
	SOCKET     _sock;

	HOSTSTRUCT _client;

public:

	 wsServer( void );
	~wsServer( void );

	bool BindAndListen( unsigned short port );
	bool Accept();

	const char* ClientName( void );

	int  Send( const char* p_buf, int data_size,                  const bool *p_b_stop );
	int  Recv(       char* p_buf, int max_recv , bool b_sizehead, const bool *p_b_stop );
	bool Shutdown();
};
