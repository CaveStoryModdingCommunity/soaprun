#define BUFSIZE_IPADDRESS 100

class ConnectTo
{

public:
	
	char ip_address[ BUFSIZE_IPADDRESS ];
	int  port_no   ;

	ConnectTo();

	bool Load();
	bool Save();
};
