#if !defined( ARGUMENT_HANDLER )
#define ARGUMENT_HANDLER
#include "archive_options.h"

class argument_handler
{
private:
	static void ShowHelp();
	static Operation GetFlag( char* flag );
	static bool CheckTargetArchName( char* name );
	static bool CheckPath( char* path );
public:
	static archive_options* ProcessArgument( int argc, char** argv );
};

#endif