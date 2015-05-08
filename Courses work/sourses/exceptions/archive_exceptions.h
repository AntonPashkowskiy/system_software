#if !defined( ARCHIVE_EXCEPTIONS )
#define ARCHIVE_EXCEPTIONS

class archive_exception
{
private:
	const char* message = nullptr;
	const char* inner_message = nullptr;
public:
	archive_exception( const char* message );
	archive_exception( const char* message, const char* sourse );
	virtual const char* GetMessage();
	virtual const char* GetInnerMessage();
};

#endif