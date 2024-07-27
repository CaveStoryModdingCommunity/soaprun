#include <StdAfx.h>

#include "../../Dx/DxDraw.h"     // Direct Draw

#include "PxImage.h"

bool Surface_Initialize( void )
{
	bool b_ret = false;

//	if( !DxDraw_SubSurface_ReadyBlank( GAMEVIEW_W, GAMEVIEW_H, SfNo_SCREENCOPY, TRUE ) ) goto End;

	if( !PxImage_Load( "COLOR_BGFIELD", SfNo_COLOR_BGFIELD, FALSE ) ) goto End;
	if( !PxImage_Load( "COLOR_BGTEXT" , SfNo_COLOR_BGTEXT , FALSE ) ) goto End;
	if( !PxImage_Load( "COLOR_TEXT"   , SfNo_COLOR_TEXT   , FALSE ) ) goto End;
	if( !PxImage_Load( "FONT_1"       , SfNo_FONT_1       , FALSE ) ) goto End;
	if( !PxImage_Load( "FRAME_B"      , SfNo_FRAME_B      , FALSE ) ) goto End;
	if( !PxImage_Load( "FRAME_H"      , SfNo_FRAME_H      , FALSE ) ) goto End;
	if( !PxImage_Load( "FRAME_M"      , SfNo_FRAME_M      , FALSE ) ) goto End;
	if( !PxImage_Load( "FRAME_V"      , SfNo_FRAME_V      , FALSE ) ) goto End;
	if( !PxImage_Load( "KEYCODE"      , SfNo_KEYCODE      , FALSE ) ) goto End;

	b_ret = true;
End:

	return b_ret;
}
