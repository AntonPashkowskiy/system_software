#include "options/argument_handler.h"
#include "options/switcher_task.h" 

int main( int argc, char** argv )
{
	archive_options* options = argument_handler::ProcessArguments( argc, argv );
	if ( options == nullptr ) return -1;
	switcher_task::RunTask( options );
	return 0;
}