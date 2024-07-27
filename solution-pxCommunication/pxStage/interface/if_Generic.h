void PutNumber6( RECT *rcView, int x, int y, int value, unsigned int flag );
// スクロールバーの表示
void Tiling_Horizontal( const RECT *p_field_rect, const RECT *p_image_rect, int rect_num, int offset, int surf );
void Tiling_Vertical(   const RECT *p_field_rect, const RECT *p_image_rect, int rect_num, int offset, int surf );
BOOL if_GenericLoad( HWND hWnd );
