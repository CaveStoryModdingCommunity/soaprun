#pragma once

class Random
{
private:

	short  _buf[ 2 ];

public :

	Random( unsigned short a, unsigned short b );
	short Get( short min_, short max_ );
};
