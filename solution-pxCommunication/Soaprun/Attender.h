#include "../ws/wsClient.h"

class Attender
{
	wsClient *_cl          ;
	char     _server_ip[ BUFFERSIZE_IPADDRESS ];
	int      _port_no      ;
	char     *_buf         ;
	char     *_mbuf        ;
	bool     _b_cl_shutdown;

public:
	 Attender();
	~Attender();

	void Reset();
	bool GetServerInformation();
	void SetServer( const char*ip, int port_no );
	bool Connection();
	bool CheckQuota   ( const bool *p_b_stop );
	bool CheckProtocol( const bool *p_b_stop );
	bool SpeedTest    ( const bool *p_b_stop );
	void RequestAndResponse( RingBuffer *_ring_req, RingBuffer *_ring_res, const bool *p_b_exit );
	void ClientShutdown();
	void ShutdownSocket();
};
