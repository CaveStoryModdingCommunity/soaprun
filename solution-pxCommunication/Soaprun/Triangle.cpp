#include <StdAfx.h>

#include "Triangle.h"

Triangle::Triangle()
{
	int i;
	float a, b;
	for( i = 0; i < 256; i ++ ) _Sin[i] = (int)( sin( (double)i * 6.2832 / 256 ) * (double)VS );

	for( i = 0; i < 33; i++ )
	{
		a = (float)( 2 * 3.14159265 ) * i / 256;
		b = (float)( sin( a ) / cos( a ) );
		_Tan[i] = (short)( b * 0x2000 );
	}
}


int Triangle::GetSin( unsigned char deg )
{
	return _Sin[ deg ];
}

int Triangle::GetCos( unsigned char deg )
{
	return _Sin[ ( deg + 64 ) % 256 ];
}

unsigned char Triangle::GetArktan( long x,long y, unsigned char def )
{
	short k;
	unsigned char a = 0;

	if( x > 0 ){
		if(       y > 0 ){
			if(  x >  y ){ k = (short)(( y*0x2000)/ x); while(k > _Tan[a])a++;              }
			else         { k = (short)(( x*0x2000)/ y); while(k > _Tan[a])a++; a =  64 - a; }
		}else if( y < 0 ){
			if(  x > -y ){ k = (short)((-y*0x2000)/ x); while(k > _Tan[a])a++; a = 256 - a; }
			else         { k = (short)(( x*0x2000)/-y); while(k > _Tan[a])a++; a = 192 + a; }
		}else{
			a = 0;
		}
	}else if( x < 0 ) {
		if(       y > 0 ) {
			if( -x >  y ){ k = (short)(( y*0x2000)/-x); while(k > _Tan[a])a++; a = 128 - a; }
			else         { k = (short)((-x*0x2000)/ y); while(k > _Tan[a])a++; a =  64 + a; }
		}else if( y < 0 ){
			if( -x > -y ){ k = (short)((-y*0x2000)/-x); while(k > _Tan[a])a++; a = 128 + a; }
			else         { k = (short)((-x*0x2000)/-y); while(k > _Tan[a])a++; a = 192 - a; }
		}else{
			a = 128;
		}
	}else{
		if(      y < 0 ) a = 192;
		else if( y > 0 ) a =  64;
		else        a = def;
	}

	return a;
}
