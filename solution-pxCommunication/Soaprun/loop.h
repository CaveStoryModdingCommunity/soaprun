#pragma once

void Loop_PutFPS( void );
BOOL Loop_Flip  ( void );


enum enum_EscapeReturn
{
	enum_EscapeReturn_Continue = 0,
	enum_EscapeReturn_Reset,
	enum_EscapeReturn_Exit
};

BOOL Loop_IsEscapeExit( void );

unsigned long Loop_GetFPS();
//bool          Loop_IsSkip();
//void Loop_Reset();
