#if !defined( MASHER_ARCHIVATOR )
#define MASHER_ARCHIVATOR
#include "archivator.h"
#include "../exceptions/archive_exceptions.h"

class masher_archivator : public archivator
{
private:
	std::vector<char*> GetAllPaths( char* root_path );
	void Archive( char* target_archive_name, std::vector<char*> paths, bool compress );
public:
	void RunArchivation( archive_options* options );
	void ExtractFiles( archive_options* options );
	void RemoveFiles( archive_options* options );
	std::vector<title_node> GetTitle( archive_options* options );
	bool ChechIntegrity( archive_options* options );
};

#endif