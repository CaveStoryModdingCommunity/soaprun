#pragma once

void KeyControl_Clear(         void );
void KeyControl_WM_MESSAGE(    HWND hWnd, UINT message, WPARAM wParam );
void KeyControl_WM_MOUSEWHEEL( HWND hWnd,               WPARAM wParam );
void KeyControl_UpdateTrigger(       void );
BOOL KeyControl_IsKey(        long vk );
BOOL KeyControl_IsKeyTrigger( long vk );
BOOL KeyControl_IsClickLeft(         void );
BOOL KeyControl_IsClickRight(        void );
BOOL KeyControl_IsClickLeftTrigger(  void );
BOOL KeyControl_IsClickRightTrigger( void );
long KeyControl_GetWheel(            void );

