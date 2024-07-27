typedef struct
{
	int         param ;
	const char* p_str ;
	char*       p_buf ;
	bool        b_dark;

}SIMPLELIST;

void SimpleList_Release(  SIMPLELIST* list, int num           );
bool SimpleList_Allocate( SIMPLELIST* list, int num, int size );
