#include "masher_archivator.h"
#include <iostream>

using namespace std;

void masher_archivator::RunArchivation( archive_options* options )
{
	vector<file_system_object> target_files;
	vector<files_tree*> trees_vector;

	for ( unsigned int i = 0; i < (options -> paths).size(); i++ )
	{
		files_tree* tree = new files_tree( (options -> paths)[ i ], options -> include_hidden_files, i );
		tree -> GetNodes( target_files );
		trees_vector.push_back( tree );
	}

	try
	{
		if( options -> requested_operation == CREATE_WITHOUT_COMPRESSING )
		{
			Archive( options -> target_archive_name, target_files, options -> comment, false );
		}
		else
		{
			Archive( options -> target_archive_name, target_files, options -> comment, true );
		}
	}
	catch( archive_exception e )
	{
		throw archive_exception( "Run archivation error.", e );
	}
}

void masher_archivator::ExtractFiles( archive_options* options )
{
	throw archive_exception( "Extracting error." );
}

void masher_archivator::RemoveFiles( archive_options* options )
{
	throw archive_exception( "Removing error." );
}

std::vector<title_node> masher_archivator::GetTitle( archive_options* options )
{
	throw archive_exception( "Title view error." );
}

bool masher_archivator::ChechIntegrity( archive_options* options )
{
	throw archive_exception( "Checking integrity error." );
}

// ------------------------------------------------------------------------

void masher_archivator::Archive (

	char* target_archive_name, 
	vector<file_system_object>& files, 
	char* comment, 
	bool compress 
)
{

}