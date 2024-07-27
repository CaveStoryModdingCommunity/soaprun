bool DebugLog_Initialize( bool bDaemon                );
void DebugLog_Release   ( void                        );
void dlog               ( const char* format, ...     );
bool DebugLog_Store     ( const char* store_directory );
