#if !defined( FILES_TREE_NODE )
#define FILES_TREE_NODE
#include "file_type.h"

#if !defined( STL_VECTOR )
#define STL_VECTOR
#include <vector>
#endif

using namespace std;

/*
	Структура представляющая собой узел файлового дерева.
	Кроме информации об узле содержит в себе вектора обычных и скрытых файлов.
*/

struct files_tree_node
{
	char* full_path;
	char* file_name;
	file_type type;
	int id;
	int parent_id;
	vector<files_tree_node> child_nodes;
	vector<files_tree_node> child_hidden_nodes;
};

#endif