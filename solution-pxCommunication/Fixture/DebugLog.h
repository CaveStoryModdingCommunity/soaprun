void DebugLog_Initialize( const char* path_dir );
void DebugLog_Release(    void     );
int  DebugLog_DeleteOlds( int days );

void dlog( const char *fmt, ... );
