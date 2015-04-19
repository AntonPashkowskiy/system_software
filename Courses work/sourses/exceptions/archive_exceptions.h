#if !defined( ARCHIVE_EXCEPTIONS )
#define ARCHIVE_EXCEPTIONS

class archive_exception
{
private:
	const char* message = nullptr;
	const char* sourse = nullptr;

public:
	archive_exception( const char* message );
	archive_exception( const char* sourse, const char* message );
	virtual void ShowMessage();
};

#endif