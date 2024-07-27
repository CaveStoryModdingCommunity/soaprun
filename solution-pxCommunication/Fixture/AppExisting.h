class AppExisting
{
private:

	HANDLE _hMapping;
	HANDLE _hMutex;
	HWND   *_p_hwnd;

	HANDLE _CreateMapping( const char *name );
	HWND   *_GetMapped   ( HANDLE h_map );

public:

	 AppExisting( void );
	~AppExisting( void );

	bool IsExisting(            const char *mutex_name, const char *mapping_name );
	bool Mapping   ( HWND hWnd, const char *mutex_name, const char *mapping_name );

};
