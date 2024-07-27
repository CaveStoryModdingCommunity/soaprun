#include <StdAfx.h>

#include "SimpleList.h"

void SimpleList_Release( SIMPLELIST* list, int num )
{
	for( int i = 0; i < num; i++ )
	{
		if( list[ i ].p_buf )
		{
			free( list[ i ].p_buf );
			list[ i ].p_buf = NULL;
		}
	}
}

bool SimpleList_Allocate( SIMPLELIST* list, int num, int size )
{
	for( int i = 0; i < num; i++ )
	{
		if( !(  list[ i ].p_buf = (char*)malloc( size ) ) ) return false;
		memset( list[ i ].p_buf, 0,              size );
		list[ i ].b_dark = false;
	}
	return true;
}