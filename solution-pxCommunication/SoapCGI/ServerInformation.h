
enum SERVERSTATUS
{
	SERVERSTATUS_Close = 0,
	SERVERSTATUS_Open,
};

#define BUFFERSIZE_IPADDRESS     64
#define BUFFERSIZE_PROTOCOL       8
#define BUFFERSIZE_SERVERCOMMENT 24
#define SERVERCOMMENT_NUM         6

typedef struct
{
	SERVERSTATUS status;
	char         ip_address[ BUFFERSIZE_IPADDRESS ];
	char         protocol  [ BUFFERSIZE_PROTOCOL  ];
	char         comments  [SERVERCOMMENT_NUM][ BUFFERSIZE_SERVERCOMMENT ];
    int          port_no;
	int          version;
}
SERVERINFORMATION;

bool ServerInformation_Save( const char *path_dst, const SERVERINFORMATION *p );
bool ServerInformation_Load( const char *path_src,       SERVERINFORMATION *p );
