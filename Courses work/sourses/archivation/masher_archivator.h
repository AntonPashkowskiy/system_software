#if !defined( MASHER_ARCHIVATOR )
#define MASHER_ARCHIVATOR
#include "archivator.h"
#include "../fsystem/fs_tree.h"
#include "../exceptions/archive_exceptions.h"
#include "../compression/LZW.h"

typedef unsigned long long size_f;

class masher_archivator : public archivator
{
private:
	// методы для архивации.
	void Archive( char* target_archive_name, std::vector<file_system_object>& files, char* comment, bool compress );
	void CheckReadAccess( vector<file_system_object>& files );
	void CreateHeaderNode( file_system_object& fs_object, header_node& header_n, size_f file_length, char compression_type );
	void WriteTotalSize( int archive_fd, size_f total_size );
	void WriteCheckSum( int archive_fd, unsigned check_sum );
	int CreateArchiveFile( char* target_archive_name );
	int WriteHeader( int archive_fd, vector<header_node>& header_nodes, bool compress );
	int WriteComment( int archive_fd, char* comment );
	header_node WriteFileToArchive( int archive_fd, file_system_object object, bool compress );
	unsigned CalculateCheckSum( int descriptor, size_f file_size );
	void DestroyTrees( vector<file_tree*>& trees );
	void GetArchiveName( char* name, char* full_name );
	// методы получения заголовка.
	archive_header CreateHeader( header_node* header_elements, unsigned int count, char* comment );
	// методы для разархивации.
	void GetFilesFromHeader( archive_header& header, vector<file_system_object>& files );
	file_system_object CreateFileObject( header_node& node );
	int TreesCount( vector<file_system_object>& files );
	void FreeFileObjects( vector<file_system_object>& files );
	void Extract( int archive_descriptor, archive_header header, vector<file_system_object>& files );
	bool DecompressFile( int archive_descriptor, size_f file_size, int file_descriptor );
	// методы для удаления файлов.
	vector<int> GetFilesIndexes( archive_header& header, archive_options* options );
	void GetAllIndexes( archive_header& header, vector<int>& indexes, int parent_index );
	void RewriteArchive( int archive_descriptor, archive_header& header, vector<int>& rf_indexes );
public:
	void RunArchivation( archive_options* options );
	void RunExtracting( archive_options* options );
	void RemoveFiles( archive_options* options );
	archive_header GetHeader( archive_options* options );
	bool CheckIntegrity( archive_options* options );
};

#endif