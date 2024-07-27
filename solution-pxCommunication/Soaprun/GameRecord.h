class GameRecord
{
private:
	char _param2;

public:
	GameRecord()
	{
		_param2 = 0;
	}

	void Set( char param2 )
	{
		_param2 = param2;
	}

	char Get() const
	{
		return _param2;
	}

};
