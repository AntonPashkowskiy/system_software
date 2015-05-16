#if !defined( FILE_TREE )
#define FILE_TREE
#include "fs_tree_node.h"
#include "fs_object.h"

using namespace std;

class file_tree
{
private:
	int tree_id = 0;
	files_tree_node root_node;
	bool with_hidden = false;
	// запрещение конструктора по умолчанию и конструктора копирования
	file_tree();
	void GetAllFiles( char* path_to_directory, vector<char*>& buffer, bool add_hidden );
	void Initialize( char* full_path, char* file_name, int& id, int parent_id, files_tree_node& node );
	void Recreate( vector<file_system_object>& nodes, files_tree_node& parent_node );
	void Dispose( files_tree_node& node );
	void GetSubnodes( files_tree_node root_node, vector<file_system_object>& nodes, bool add_hidden );
	void MapNode( files_tree_node& node, file_system_object& object );
	void MapObject( file_system_object& object, files_tree_node& node );
	char* CreatePath( char* root_path, char* file_path );
	char* GetFileName( char* path );
	void CreateFiles( files_tree_node& node );
public:
	file_tree( char* root_path, bool with_hidden, int tree_id );
	file_tree( vector<file_system_object>& nodes );
	~file_tree();

	void GetNodes( vector<file_system_object>& nodes );
	void CreateFilesTree();
};

#endif