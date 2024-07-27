class CriticalSection
{
private:
	
	CRITICAL_SECTION _cs;
	bool             _b ;

public :

	CriticalSection( void );
	~CriticalSection( void );
	bool In();
	void Out();
	void End();
};
