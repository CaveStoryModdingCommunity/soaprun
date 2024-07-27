class TextReader
{
private:

	const char *_p_buf;
	int        _size;
	int        _c;

public:

	TextReader( const char *p_buf, int size );
	bool toFind( const char* str );
	bool toNextLine( void );
	bool GetOneLine( char *dst, int size );
};
