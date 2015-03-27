#if !defined( ARCHIVE_EXCEPTIONS )
#define ARCHIVE_EXCEPTIONS

class archive_exception
{
private:
	char* message;
	char* sourse;

public:
	archive_exception( char* message );
	archive_exception( char* sourse, char* message );
	virtual void ShowMessage();
};

#endif