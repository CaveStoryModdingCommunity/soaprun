#include <StdAfx.h>

#include "SharedDefines.h"
#include "Weather.h"

void Weather::_Set_RainDrop()
{
	int x = _ran->Get( 0, _w );
	int y = _ran->Get( 0, _h ) - (40*4);
//		int x = 10;
//		int y = - (0);

	for( int c = 0; c < _caret_num; c++ )
	{
		if( _carets[ c ]->IsDisable() ){ _carets[ c ]->Set( CARET_RainDrop, x, y ); break; }
	}
}

Weather::Weather( int w, int h, int caret_num, Random *ran )
{
	_caret_num = caret_num;
	_w         = w        ;
	_h         = h        ;
	_ran       = ran      ;
	_pane = new Pane( caret_num );
	_pane->SetType_View( 0, 0, w, h ); 

	_carets = (Caret**)malloc( sizeof(Caret*) * caret_num );
	memset( _carets, 0, sizeof(Caret*) * caret_num );

	for( int c = 0; c < caret_num; c++ )
	{
		_carets[ c ] = new Caret();
		_pane->Child_Add( _carets[ c ]->Pane_Get() );
	}

	_weather = WEATHER_None;
}

Pane *Weather::Pane_Get()
{
	return _pane;
}

Weather::~Weather()
{
	if( _carets )
	{
		for( int c = 0; c < _caret_num; c++ )
		{
			if( _carets[ c ] ) delete _carets[ c ];
		}
		free( _carets );
	}
}

void Weather::Set( WEATHER wt )
{
	_weather = wt;
	if( _weather )
		wt = wt;
}

void Weather::Procedure()
{
	switch( _weather )
	{
	case WEATHER_Rain: if( (++_wt_ct)%2 == 0 ) _Set_RainDrop(); break;
	}

	for( int c = 0; c < _caret_num; c++ ) _carets[ c ]->Action();
}

bool Weather::IsRain()
{
	if( _weather == WEATHER_Rain ) return true;
	return false;
}
