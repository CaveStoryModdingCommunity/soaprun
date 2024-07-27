#include <StdAfx.h>

#include "Caret.h"

void Caret::_Action_RainDrop()
{
	_act_count++;
	if( _act_count < 20 )
	{
		_y += VS * 8;
	}
	else if( _act_count == 20 )
	{
		_pane->SetAnimation ( 1, 0, 0, 0 );
	}
	else if( _act_count > 23 )
	{
		_type = CARET_Disable;
		_pane->Visible( false );
	}
}

Caret::Caret()
{
	_pane = new Pane();
	_type = CARET_Disable;
}

Caret::~Caret()
{
	if( _pane ) delete _pane;
}

Pane *Caret::Pane_Get()
{
	return _pane;
}

void Caret::Set( CARETTYPE type, int x, int y )
{
	switch( type )
	{
	case CARET_RainDrop:

		_pane->SetType_Image( x, y, 8, 8, SfNo_Weather );
		_pane->SetAnimation ( 0, 0, 0, 0 );
		_hfW = 4;
		_hfH = 4;
		_x   = x * VS;
		_y   = y * VS;
		break;

	default: return;
	}

	_type      = type;
	_act_count =    0;
}

void Caret::Reset()
{
	_type = CARET_Disable;
}

void Caret::Action()
{
	switch( _type )
	{
	case CARET_RainDrop: _Action_RainDrop(); break;

	default : return;
	}
	_pane->SetPos( _x / VS - _hfW, _y / VS - _hfH );
}

bool Caret::IsDisable()
{
	if( _type == CARET_Disable ) return true;
	return false;
}
