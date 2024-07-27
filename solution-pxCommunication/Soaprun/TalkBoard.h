#include "../Fixture/RingBuffer.h"

#include "utility_Draw/MessageFrame.h"


class TalkBoard
{
	enum
	{
		_BUFFERSIZE_PUSH =  256,
	};

	MessageFrame *_mf   ;
	RingBuffer   *_ring ;
	char         *_r_buf;
	int          _r     ;
	int          _len   ;

public:

	TalkBoard( RingBuffer *ring );
	~TalkBoard();
	void Procedure();
	void Put( const RECT *rc_view );
	void Clear();
	bool IsBusy();
};

