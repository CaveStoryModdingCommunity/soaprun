#include <StdAfx.h>

bool URL_Encode( const char *src, char *dst, int dst_size )
{
	int len = strlen( src );
	int d   = 0;

	for( int s = 0; s < len; s++, d++ )
	{
		unsigned char c = (unsigned char)src[ s ];

		if     ( c >= '0' && c <= '9'                         ) dst[ d ] = c;
		else if( c >= 'a' && c <= 'z'                         ) dst[ d ] = c;
		else if( c >= 'A' && c <= 'Z'                         ) dst[ d ] = c;
		else if( c == '_' || c == '-' || c == '.' || c == '*' ) dst[ d ] = c;
		else if( c == ' '                                     ) dst[ d ] = '+';
		else if( c >= 0x20 && c < 0x7F ){ sprintf( &dst[ d ], "%%%02x", c ); d += 2; } // ‚»‚Ì‘¼‚Ì”¼Šp‹L†
		else if( c >= 0xA1 && c < 0xE0 ){ sprintf( &dst[ d ], "%%%02x", c ); d += 2; } // ”¼ŠpƒJƒi
		else // ‚»‚Ì‘¼(‘SŠp)
		{
			sprintf( &dst[ d ], "%%%02X%%%02X",
				(unsigned char)src[ s     ],
				(unsigned char)src[ s + 1 ] );
			d += 5;
			s++;
		}
	}
	if( d >= dst_size ) return false;

	dst[ d ] = '\0';
	return true;
}

bool URL_Decode( const char *src, char *dst, int dst_size )
{
	int len = strlen( src );
	int d   = 0;

	for( int s = 0; s < len; s++, d++ )
	{
		unsigned char c = (unsigned char)src[ s ];

		if( c == '%' )
		{
			char work[ 3 ];
			work[ 0 ] = (unsigned char)src[ s + 1 ];
			work[ 1 ] = (unsigned char)src[ s + 2 ];
			work[ 2 ] = '\0';
			sscanf( work, "%02X", &dst[ d ] );
			s += 2;
		}
		else if( c == '+' ) dst[ d ] = ' ';
		else                dst[ d ] = c  ;
	}
	dst[ d ] = '\0';

	if( d >= dst_size ) return false;

	dst[ d ] = '\0';
	return true;
}
