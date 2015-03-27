#if !defined( ARGUMENT_HANDLER )
#define ARGUMENT_HANDLER
#include "archive_options.h"

class argument_handler
{
private:
	static Operation GetFlag( char* flag );
	static void ShowHelp();
	static bool CheckTargetArchiveName( char* name );
	static bool CheckPath( char* path );
	static bool IsDeniedSymbol( char symbol, bool is_path );
public:
	static archive_options* ProcessArgument( int argc, char** argv );
	static void Test( archive_options* options );
};

#endif