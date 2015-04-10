#include "mahser_archivator.h"

void masher_archivator::RunArchivation( archive_options* options )
{
	throw archive_exception( "Run archivation exception." );
}

void mahser_archivator::ExtractingFiles( archive_options* options )
{
	throw archive_exception( "Extracting exception." );
}

void mahser_archivator::RemoveFiles( archive_options* options )
{
	throw archive_exception( "Removing exception." );
}

std::vector<title_node> masher_archivator::GetTitle( archive_options* options )
{
	throw archive_exception( "Title getting exception." );
}

bool masher_archivator::ChechIntegrity( archive_options* options )
{
	throw archive_exception( "Integrity checking exception." );
}