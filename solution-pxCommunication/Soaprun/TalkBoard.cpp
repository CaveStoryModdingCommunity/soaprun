#include <StdAfx.h>

#include "TalkBoard.h"

TalkBoard::TalkBoard( RingBuffer *ring )
{
	_r     = 0;
	_len   = 0;
	_ring  = ring;
	_mf    = new MessageFrame( 16, 14, POSFROM_LT, FIELDVIEW_W, 24 );
	_r_buf = (char*)malloc ( _BUFFERSIZE_PUSH );
	memset( _r_buf, 0, _BUFFERSIZE_PUSH );

	_mf->SetVisible    ( true  );
	_mf->Cursor_Visible( false );
}

TalkBoard::~TalkBoard()
{
	delete _mf;
	if( _r_buf ) free( _r_buf );
}

void TalkBoard::Procedure()
{
	if( _r >= _len )
	{
		if( _ring->txt_Pop( _r_buf ) )
		{
			if( !_mf->IsEmpty() ) _mf->SetBreak();
			_r   = 0;
			_len = strlen( _r_buf );
		}
	}
	if( _r_buf[ _r ] == '\n' ){ _mf->SetBreak(); _r++; }
	else
	{
		int inc = _mf->SetCell( (const unsigned char*)&_r_buf[ _r ], _BUFFERSIZE_PUSH - _r );
		if( inc ) _r += inc;
	}
	_mf->Procedure();
}

void TalkBoard::Put( const RECT *rc_view )
{
	_mf->Put( rc_view );
}

void TalkBoard::Clear()
{
	_r     = 0;
	_len   = 0;
	_mf->Clear();
}

bool TalkBoard::IsBusy()
{
	if( _r >= _len ) return false;
	return true;
}


