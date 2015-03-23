#if !defined( ARCHIVE_EXCEPTIONS )
#define ARCHIVE_EXCEPTIONS

class exception
{
private:
	char* message;
	char* sourse;

public:
	exception( char* message );
	exception( char* sourse, char* message );
	virtual void ShowMessage();
};

#endif