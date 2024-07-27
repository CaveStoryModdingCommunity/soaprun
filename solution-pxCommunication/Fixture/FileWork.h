bool FileWork_IsFileStable(     const char* path, int sec_stable );
bool FileWork_IsDirectoryEmpty( const char* path_dir );
void FileWork_ClearDirectory(   const char* path_dir );
bool FileWork_MoveFile        ( const char* path_src, const char* path_dst, bool b_force );
bool FileWork_ForceDelete     ( const char* path_src );
