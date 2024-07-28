int  lnxSock_Recv(       char *buf, int buf_size , int sock, bool b_sizehead, const bool *p_b_stop );
//bool lnxSock_Send( const char *buf, int data_size, int sock, bool b_sizehead, const bool *p_b_stop );
int lnxSock_Send( const char *p, int data_size, int sock, const bool *p_b_stop );
int lnxSock_Send_mbuf( char *mbuf, int data_size, int sock, const bool *p_b_stop );
