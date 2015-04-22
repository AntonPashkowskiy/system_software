#include "fs_tree.h"
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <iostream>

using namespace std;

files_tree::files_tree( char* root_path, bool with_hidden )
{
	int root_id = 0;
	// флаг необходимости добавлять скрытые файлы
	this -> with_hidden = with_hidden;
	Initialize( root_path, nullptr, root_id, 0, root_node );
}

files_tree::~files_tree()
{
	Dispose( root_node );
}

void files_tree::GetAllFiles( char* path_to_directory, vector<char*>& buffer, bool add_hidden )
{
	DIR* directory = opendir( path_to_directory );
	char* file_name = nullptr;
	int length = 0;

	if( directory == nullptr )
	{
		return;
	}

	dirent* file = nullptr;
	
	do
	{
		file = readdir( directory );

		// не добавляет родительскую директорию, саму себя и если не
		// выставлен флаг - скрытые файлы
		if( file != nullptr )
		{
			if( strcmp( file -> d_name, ".." ) != 0 && 
				strcmp( file -> d_name, "." ) != 0 &&
				!( !add_hidden && (file -> d_name)[0] == '.' )
			  )
			{
				length = strlen( file -> d_name );
				file_name = new char[ length + 1 ];
				strcpy( file_name, file -> d_name );
				buffer.push_back( file_name );
			}	
		}
	}
	while( file != nullptr );

	closedir( directory );
}

char* files_tree::CreatePath( char* root_path, char* file_path )
{
	if( root_path == nullptr || file_path == nullptr )
	{
		return nullptr;
	}

	int length_r = strlen( root_path );
	int length_f = strlen( file_path );

	char* path =  new char[ length_r + length_f + 4 ];
	strcpy( path, root_path );
	strcat( path, "/" );
	strcat( path, file_path );
	return path;
}

char* files_tree::GetFileName( char* path )
{
	if( path == nullptr )
	{
		return nullptr;
	}

	int path_length = strlen( path );
	int name_length = 0;
	char* name = nullptr;

	for( int i = path_length - 1; i >= 0; i-- )
	{
		if( path[ i ] == '/' )
		{
			name_length = strlen( path + i + 1 );
			name = new char[ name_length + 1 ];
			strcpy( name, path + i + 1 );
			return name;
		}
	}

	name = new char[ path_length + 1 ];
	strcpy( name, path );
	return name;
}

void files_tree::Initialize( char* full_path, char* file_name, int& id, int parent_id, files_tree_node& node )
{
	if( full_path == nullptr )
	{
		return;
	}

	if( file_name == nullptr )
	{
		char* name = GetFileName( full_path );
		node.file_name = name;
	}
	else
	{
		node.file_name = file_name;
	}

	node.full_path = full_path;
	node.id = id;
	node.parent_id = parent_id;
	struct stat* information = new struct stat;

	if( stat( full_path, information ) == -1 )
	{
		return;
	}

	if( S_ISREG( information -> st_mode ) )
	{
		node.type = REGULAR;
	}
	else if( S_ISDIR( information -> st_mode ) )
	{
		node.type = DIRECTORY;
		vector<char*> subfiles;
		char* path = nullptr;
		int temp = id;

		GetAllFiles( full_path, subfiles, this -> with_hidden );

		for ( unsigned int i = 0; i < subfiles.size(); i++ )
		{
			path = CreatePath( full_path, subfiles[ i ] );
			id ++;
			files_tree_node child_node;
			Initialize( path, subfiles[ i ], id, temp, child_node );

			if( subfiles[ i ][ 0 ] == '.' )
			{
				node.child_hidden_nodes.push_back( child_node );
			}
			else
			{
				node.child_nodes.push_back( child_node );
			}
		}

		subfiles.clear();
	}
}

void files_tree::Dispose( files_tree_node& node )
{
	if( node.file_name != nullptr && node.id != 0 )
	{
		delete node.full_path;
		delete node.file_name;
	}

	for( unsigned int i = 0; i < node.child_nodes.size(); i++ )
	{
		Dispose( node.child_nodes[ i ] );
	}

	for( unsigned int i = 0; i < node.child_hidden_nodes.size(); i++ )
	{
		Dispose( node.child_hidden_nodes[ i ] );
	}

	node.child_nodes.clear();
	node.child_hidden_nodes.clear();
}

void files_tree::GetNodes( vector<file_system_object>& nodes )
{
	file_system_object object;
	// записываем информацию о корне
	MapNode( root_node, object );
	nodes.push_back( object );
	// запуск рекурсивной функции
	GetSubnodes( root_node, nodes, this -> with_hidden );
}

void files_tree::GetSubnodes( files_tree_node node, vector<file_system_object>& nodes, bool add_hidden )
{
	file_system_object object;

	for( unsigned int i = 0; i < node.child_nodes.size(); i++ )
	{
		MapNode( node.child_nodes[ i ], object );
		nodes.push_back( object );
		GetSubnodes( node.child_nodes[ i ], nodes, add_hidden );
	}

	if( add_hidden )
	{
		for( unsigned int i = 0; i < node.child_hidden_nodes.size(); i++ )
		{
			MapNode( node.child_hidden_nodes[ i ], object );
			nodes.push_back( object );
			GetSubnodes( node.child_hidden_nodes[ i ], nodes, add_hidden );
		}
	}
}

void files_tree::MapNode( files_tree_node& node, file_system_object& object )
{
	object.full_path = node.full_path;
	object.file_name = node.file_name;
	object.type = node.type;
	object.object_id = node.id;
	object.parent_id = node.parent_id;
}