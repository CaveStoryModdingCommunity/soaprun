#include "ws.h"

class wsClient
{
private:

	HOSTSTRUCT _host;

public :

	 wsClient();
	~wsClient();

	char           server_address[ MAX_HOSTNAME ];
	unsigned short server_port;

	bool Connect( const char *ip_address, unsigned short port, int *p_wsa_err );
	int  Send     ( const char *p_buf, int data_size,                  const bool *p_b_stop );
	int  Send_mbuf(       char *mbuf , int data_size,                  const bool *p_b_stop );
	int  Recv     (       char *p_buf, int max_recv , bool b_sizehead, const bool *p_b_stop );
	bool Shutdown();
//	bool Shutdown( char *p_buf, int max_recv, const bool *p_b_stop, int sec_timeout );
};
