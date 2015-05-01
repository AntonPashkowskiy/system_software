#if !defined( ARCHIVE_EXCEPTIONS )
#define ARCHIVE_EXCEPTIONS

class archive_exception
{
private:
	const char* message = nullptr;
	archive_exception* inner_exception = nullptr;
public:
	archive_exception( const char* message );
	archive_exception( const char* message, archive_exception& inner_exception );
	virtual const char* GetMessage();
	virtual archive_exception GetInnerException();
};

#endif