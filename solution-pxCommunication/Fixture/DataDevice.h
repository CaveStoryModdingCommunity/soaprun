#pragma once

typedef struct
{
	BOOL          bFile;
	FILE          *fp;
	unsigned char *p;
	long          offset;
	long          size;

}DDV;

// fopen();  type_name:"EXTERIOR" で外部ファイルを開く
BOOL ddv_Open( HMODULE hModule, const char *file_name, const char *type_name, DDV *p_read );
// fread();
BOOL ddv_Read( void *p, long size, long num, DDV *p_read );
// fseek();
BOOL ddv_Seek( DDV *p_read, long offset, long mode );
// fclose();
void ddv_Close( DDV *p_read );


long ddv_Variable_CheckSize( long value );
long ddv_Variable_Write(         long value, FILE *fp, long *p_add );
BOOL ddv_Variable_Read(          long  *p, DDV *p_read );
BOOL ddv_Variable_Read(          short *p, DDV *p_read );
BOOL ddv_Variable_Read(          char  *p, DDV *p_read );
BOOL ddv_Variable_Read( unsigned short *p, DDV *p_read );


bool ddv_String_Write( const char* str, unsigned char buf_size, FILE *fp     );
bool ddv_String_Read(        char* str, unsigned char buf_size, DDV  *p_read );
