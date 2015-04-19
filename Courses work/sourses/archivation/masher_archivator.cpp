#include "masher_archivator.h"

#define RUN_EXCEPTION_MESSAGE " Run archivation exception."
#define EXTRACTING_EXCEPTION_MESSAGE " Extracting exception."
#define REMOVING_EXCEPTION_MESSAGE " Removing exception."
#define TITLE_EXCEPTION_MESSAGE " Title getting exception."
#define CHECK_EXCEPTION_MESSAGE " Integrity checking exception." 

using namespace std;

void masher_archivator::RunArchivation( archive_options* options )
{
	/*int archive_descriptor = open( options -> target_archive_name, O_CREAT | O_WRONLY | O_TRUNC );

	if( archive_descriptor == -1 )
	{
		throw new archive_exception( RUN_EXCEPTION_MESSAGE );
	}*/

	vector<char*> target_paths;
	vector<char*> temp;

	for ( int i = 0; i < (options -> paths).size(); i++ )
	{
		temp = GetAllPaths( (options -> paths)[ i ] );

		for( int j = temp.size() - 1; j >= 0; j-- )
		{
			target_paths.push_back( temp.back() );
			temp.pop_back();
		}
	}

	if( options -> requested_operation == CREATE_WITHOUT_COMPRESSING )
	{
		Archive( options -> target_archive_name, target_paths, false );
	}
	else
	{
		Archive( options -> target_archive_name, target_paths, true );
	}
}

void masher_archivator::ExtractFiles( archive_options* options )
{
	throw archive_exception( EXTRACTING_EXCEPTION_MESSAGE );
}

void masher_archivator::RemoveFiles( archive_options* options )
{
	throw archive_exception( REMOVING_EXCEPTION_MESSAGE );
}

std::vector<title_node> masher_archivator::GetTitle( archive_options* options )
{
	throw archive_exception( TITLE_EXCEPTION_MESSAGE );
}

bool masher_archivator::ChechIntegrity( archive_options* options )
{
	throw archive_exception( CHECK_EXCEPTION_MESSAGE );
}