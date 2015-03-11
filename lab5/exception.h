#if !defined( EXCEPTION_HANDLING )
#define EXCEPTION_HANDLING
#endif

class exception
{
private:
	char* _message;
	char* _called_function;
public:
	exception( char* message );
	exception( char* message, char* called_function );
	
	void Log( int descriptor, char* message, char* called_function );
	void Message();
};
