#if !defined( MASHER_ARCHIVATOR )
#define MASHER_ARCHIVATOR
#include "archivator.h"
#include "../fsystem/fs_tree.h"
#include "../exceptions/archive_exceptions.h"

#if !defined( STD_LIST )
#define STD_LIST
#include <list>
#endif

class masher_archivator : public archivator
{
private:
	void Archive( char* target_archive_name, std::vector<file_system_object>& files, bool compress );
public:
	void RunArchivation( archive_options* options );
	void ExtractFiles( archive_options* options );
	void RemoveFiles( archive_options* options );
	std::vector<title_node> GetTitle( archive_options* options );
	bool ChechIntegrity( archive_options* options );
};

#endif