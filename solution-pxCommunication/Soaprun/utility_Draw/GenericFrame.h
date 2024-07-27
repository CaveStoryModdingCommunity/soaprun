#pragma once

//#include "../../Dx/DxDraw.h"

#define GENERICFRAME_W    8
#define GENERICFRAME_H    8
#define FRAMEIMAGE_H_W  320

enum GENERICFRAMEACTION
{
	GENERICFRAME_STOP ,
	GENERICFRAME_FLASH,
	GENERICFRAME_SHOCK,
};

class GenericFrame
{
private :
	RECT               _rect      ;
	unsigned long      _back_color;

	int                _off_x     ;
	int                _off_y     ;
	int                _ani_no    ;
	int                _ani_wait  ;
	GENERICFRAMEACTION _action    ;

public:
	GenericFrame( const RECT* p_rect );
	void SetRect( const RECT* p_rect );
	void SetAction( GENERICFRAMEACTION action );
	void Action( void );
	void Put( const RECT* rc_view );
};
