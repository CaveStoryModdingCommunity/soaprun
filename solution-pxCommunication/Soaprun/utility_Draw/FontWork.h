#define FONTWORK_FONTW_W         8
#define FONTWORK_FONTW_C         5
#define FONTWORK_FONTH          12

#define FONTFLAG_DARK         0x80

unsigned short FontWork_GetWidth( unsigned short code );
void FontWork_Put       ( const RECT* p_rc_view, int x, int y, unsigned short code, int flags );
int  FontWork_PutNumber6( const RECT *p_rc_view, int x, int y, int value, int flags );
int  FontWork_PutText   ( const RECT* p_rc_view, int x, int y, const char* text, int flags );
