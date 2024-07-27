#include <StdAfx.h>

#include "DataDevice.h"

// fopen();
BOOL ddv_Open( HMODULE hModule, const char *file_name, const char *type_name, DDV *p_read )
{
	memset( p_read, 0, sizeof(DDV) );

	// 外部ファイル
	if( !type_name )
	{
		p_read->fp    = fopen( file_name, "rb" );
		if( !p_read->fp ) return FALSE;
		p_read->bFile = TRUE;
	}
	// 内部リソース
	else
	{
		HRSRC   hResource;
		HGLOBAL hGlobal;
		hResource     = FindResource(   hModule, file_name, type_name ); if( !hResource    ) return FALSE;
		p_read->size  = SizeofResource( hModule, hResource );            if( !p_read->size ) return FALSE;
		hGlobal       = LoadResource(   hModule, hResource );            if( !hGlobal      ) return FALSE;
		p_read->p     = (unsigned char *)LockResource( hGlobal );        if( !p_read->p    ) return FALSE;
		p_read->bFile = FALSE;
	}

	return TRUE;
}

// fread();
BOOL ddv_Read( void *p, long size, long num, DDV *p_read )
{
	if( p_read->bFile ){
		if( fread( p, size, num, p_read->fp ) != num ) return FALSE;
	}else{
		for( long i = 0; i < num; i++ ){
			if( p_read->offset + size > p_read->size ) return FALSE;
			memcpy( &((char*)p)[ i ], &p_read->p[ p_read->offset ], size );
			p_read->offset += size;
		}
	}
	return TRUE;
}


BOOL ddv_Seek( DDV *p_read, long offset, long mode )
{
	if( p_read->bFile ){
		if( fseek( p_read->fp, offset, mode ) ) return FALSE;
	}else{
		switch( mode ){
		case SEEK_SET:
			if( offset >= p_read->size ) return FALSE;
			p_read->offset = offset;
			break;
		case SEEK_CUR:
			if( p_read->offset + offset >= p_read->size ) return FALSE;
			p_read->offset += offset;
			break;
		case SEEK_END:
			return FALSE;
		}
	}
	return TRUE;
}

// fclose();
void ddv_Close( DDV *p_read )
{
	if( p_read->bFile ){
		if( p_read->fp ){
			fclose( p_read->fp );
			p_read->fp = NULL;
		}
	}else{
		if( p_read->p ) p_read->p = NULL;
	}
}


// 可変長の長さを取得する
long ddv_Variable_CheckSize( long value )
{
	unsigned long us;

	us = (unsigned long)value;
	// 1byte(7bit)
	if( us <        0x80 ) return 1;
	// 2byte(14bit)
	if( us <      0x4000 ) return 2;
	// 3byte(21bit)
	if( us <    0x200000 ) return 3;
	// 4byte(28bit)
	if( us <  0x10000000 ) return 4;
	// 5byte(35bit)
//	if( value < 0x800000000 ) return 5;
	if( us <= 0xffffffff ) return 5;

	return 6;
}



// 可変長書き込み（unsigned long までを保証）
long ddv_Variable_Write( long value, FILE *fp, long *p_add )
{
	unsigned char a[5];
	unsigned char b[5];
	unsigned long us;

	us = (unsigned long)value;
	
	a[ 0 ] = *( (unsigned char *)(&us) + 0 );
	a[ 1 ] = *( (unsigned char *)(&us) + 1 );
	a[ 2 ] = *( (unsigned char *)(&us) + 2 );
	a[ 3 ] = *( (unsigned char *)(&us) + 3 );
	a[ 4 ] = 0;

	// 1byte(7bit)
	if( us < 0x80 ){
		if( fwrite( &a[0], 1, 1, fp ) != 1 ) return FALSE;
		if( p_add ) *p_add += 1;
		return TRUE;
	}

	// 2byte(14bit)
	if( us < 0x4000 ){
		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F);
		if( fwrite( b, 1, 2, fp )     != 2 ) return FALSE;
		if( p_add ) *p_add += 2;
		return TRUE;
	}

	// 3byte(21bit)
	if( us < 0x200000 ){
		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F) | 0x80;
		b[2] = (a[1]>>6) | ((a[2]<<2)&0x7F);
		if( fwrite( b, 1, 3, fp )     != 3 ) return FALSE;
		if( p_add ) *p_add += 3;
		return TRUE;
	}

	// 4byte(28bit)
	if( us < 0x10000000 ){
		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F) | 0x80;
		b[2] = (a[1]>>6) | ((a[2]<<2)&0x7F) | 0x80;
		b[3] = (a[2]>>5) | ((a[3]<<3)&0x7F);
		if( fwrite( b, 1, 4, fp )     != 4 ) return FALSE;
		if( p_add ) *p_add += 4;
		return TRUE;
	}

	// 5byte(35bit)
//	if( value < 0x800000000 ){
	if( us <= 0xffffffff ){

		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F) | 0x80;
		b[2] = (a[1]>>6) | ((a[2]<<2)&0x7F) | 0x80;
		b[3] = (a[2]>>5) | ((a[3]<<3)&0x7F) | 0x80;
		b[4] = (a[3]>>4) | ((a[4]<<4)&0x7F);
		if( fwrite( b, 1, 5, fp )     != 5 ) return FALSE;
		if( p_add ) *p_add += 5;
		return TRUE;
	}

	return FALSE;
}


// 可変長読み込み（unsigned long までを保証）
BOOL ddv_Variable_Read( long *pValue, DDV *p_read )
{
	long i;
	unsigned char a[5];
	unsigned char b[5];

	b[0] = b[1] = b[2] = b[3] = b[4] = 0;

	for( i = 0; i < 5; i++ ){
		if( !ddv_Read( &a[i], 1, 1, p_read ) ) return FALSE;
		if( !(a[i] & 0x80) ) break;
	}
	switch( i ){
	case 0:
		b[0]    =  (a[0]&0x7F)>>0;
		break;
	case 1:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    =  (a[1]&0x7F)>>1;
		break;
	case 2:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    = ((a[1]&0x7F)>>1) | (a[2]<<6);
		b[2]    =  (a[2]&0x7F)>>2;
		break;
	case 3:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    = ((a[1]&0x7F)>>1) | (a[2]<<6);
		b[2]    = ((a[2]&0x7F)>>2) | (a[3]<<5);
		b[3]    =  (a[3]&0x7F)>>3;
		break;
	case 4:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    = ((a[1]&0x7F)>>1) | (a[2]<<6);
		b[2]    = ((a[2]&0x7F)>>2) | (a[3]<<5);
		b[3]    = ((a[3]&0x7F)>>3) | (a[4]<<4);
		b[4]    =  (a[4]&0x7F)>>4;
		break;
	case 5:
		return FALSE;
	}

	*pValue = *((long *)b);

	return TRUE;
	
}

BOOL ddv_Variable_Read( unsigned short *p, DDV *p_read )
{
	long l;
	if( !ddv_Variable_Read( &l, p_read ) ) return FALSE;
	if( l >= 0x10000 ) return FALSE;
	*p = (short)l;
	return TRUE;
}

BOOL ddv_Variable_Read( short *p, DDV *p_read )
{
	long l;
	if( !ddv_Variable_Read( &l, p_read ) ) return FALSE;
	if( l >= 0x8000 ) return FALSE;
	*p = (short)l;
	return TRUE;
}

BOOL ddv_Variable_Read( char  *p, DDV *p_read )
{
	long l;
	if( !ddv_Variable_Read( &l, p_read ) ) return FALSE;
	if( l >= 0x80 ) return FALSE;
	*p = (char)l;
	return TRUE;
}


bool ddv_String_Write( const char* str, unsigned char buf_size, FILE* fp )
{
	if( !fp || !str ) return false;
	int len = strlen( str );
	if( buf_size && len >= buf_size ) return false;

	unsigned char len_write = len;
	if( fwrite( &len_write, sizeof(unsigned char),   1, fp ) !=   1 ) return false;
	if( fwrite(  str      , sizeof(         char), len, fp ) != len ) return false;

	return true;
}

bool ddv_String_Read( char* str, unsigned char buf_size, DDV *p_read )
{
	unsigned char len;

	if( !p_read || !str ) return false;
	if( !ddv_Read( &len, sizeof(unsigned char),   1, p_read ) ) return false;
	if( len >= buf_size ) return false;
	if( !ddv_Read(  str, sizeof(         char), len, p_read ) ) return false;
	str[ len ] = '\0';

	return true;
}

/*
void test(void)
{
	FILE *fp;

	unsigned long value1[4];
	unsigned long value2[4];

	value1[0] = 0xe224;
	value1[1] = 0x0012;
	value1[2] = 0x0100;

	fp = fopen( "c:\\test", "wb" );
	ddv_Variable_Write( value1[0], fp );
	ddv_Variable_Write( value1[1], fp );
	ddv_Variable_Write( value1[2], fp );
	fclose( fp );

	fp = fopen( "c:\\test", "rb" );
	ddv_Variable_Read( &value2[0], fp );
	ddv_Variable_Read( &value2[1], fp );
	ddv_Variable_Read( &value2[2], fp );
	fclose( fp );

	value2[0] = value2[0];
	value2[1] = value2[1];
	value2[2] = value2[2];
}

*/