#pragma once

class Triangle
{
	int   _Sin[256];
	short _Tan[ 33];

public:

	Triangle();
	int GetSin( unsigned char deg );
	int GetCos( unsigned char deg );

	unsigned char GetArktan( long x,long y, unsigned char def );
};
