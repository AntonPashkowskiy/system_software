#include "files_functions.h"
#include "exception.h"

fileSearcher::fileSearcher( char* search_mask )
{
	if( search_mask != NULL )
	{
		_search_mask = search_mask;
	}
	else
	{
		throw exception( "Invalid search mask.", "fileSearcher" );
	}
}

int fileSearcher::GetNextFile()
{
	return 0;
}


