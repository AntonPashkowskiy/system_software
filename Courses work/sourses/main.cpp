#include "handlers/argument_handler.h"
#include "exceptions/archive_exceptions.h"

int main( int argc, char** argv )
{
	try
	{
		archive_options* options = argument_handler::ProcessArgument( argc, argv );

		if( options == nullptr )
		{
			return -1;
		}
	}
	catch( exception ex )
	{
		ex.ShowMessage();
	}

	return 0;
}