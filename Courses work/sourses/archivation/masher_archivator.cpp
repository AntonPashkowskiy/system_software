#include "masher_archivator.h"
#include <iostream>

#define RUN_EXCEPTION_MESSAGE " Run archivation error."
#define EXTRACTING_EXCEPTION_MESSAGE " Extracting error."
#define REMOVING_EXCEPTION_MESSAGE " Removing error."
#define TITLE_EXCEPTION_MESSAGE " Title getting error."
#define CHECK_EXCEPTION_MESSAGE " Integrity checking error." 

using namespace std;

void masher_archivator::RunArchivation( archive_options* options )
{
	vector<file_system_object> target_files;
	vector<files_tree*> trees_vector;

	for ( unsigned int i = 0; i < (options -> paths).size(); i++ )
	{
		files_tree* tree = new files_tree( (options -> paths)[ i ], options -> include_hidden_files );
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
		e.ShowMessage();
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

// ------------------------------------------------------------------------

void masher_archivator::Archive (

	char* target_archive_name, 
	std::vector<file_system_object>& files, 
	char* comment, 
	bool compress 
)
{
	cout << "Comment: " << comment << "\n\n";
	for( unsigned int i = 0; i < files.size(); i++ )
	{
		cout << files[ i ].full_path << endl;
	}
}