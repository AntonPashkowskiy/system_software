#if !defined( MASHER_ARCHIVATOR )
#define MASHER_ARCHIVATOR
#include "archivator.h"
#include "../fsystem/fs_tree.h"
#include "../exceptions/archive_exceptions.h"

typedef unsigned long long size_f;

class masher_archivator : public archivator
{
private:
	// методы для архивации
	void Archive( char* target_archive_name, std::vector<file_system_object>& files, char* comment, bool compress );
	int CreateArchiveFile( char* target_archive_name );
	void CheckReadAccess( vector<file_system_object> files );
	title_node WriteFileToArchive( int archive_fd, file_system_object object, bool compress );
	int WriteTitle( int archive_fd, vector<title_node> title, bool compress );
	int WriteComment( int archive_fd, char* comment );
	void WriteTotalSize( int archive_fd, size_f total_size );
	unsigned CalculateCheckSum( int descriptor, size_f file_size );
	void WriteCheckSum( int archive_fd, unsigned check_sum );
	// методы получения заголовка
	archive_title CreateTitle( title_node* title_elements, int count, char* comment );
public:
	void RunArchivation( archive_options* options );
	void RunExtracting( archive_options* options );
	void RemoveFiles( archive_options* options );
	archive_title GetTitle( archive_options* options );
	bool CheckIntegrity( archive_options* options );
};

#endif