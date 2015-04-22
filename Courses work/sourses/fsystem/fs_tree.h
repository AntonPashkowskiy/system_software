#if !defined( FILES_TREE )
#define FILES_TREE
#include "fs_tree_node.h"
#include "fs_object.h"

using namespace std;

class files_tree
{
private:
	files_tree_node root_node;
	// добавлять ли скрытые файлы
	bool with_hidden;
	// запрещение конструктора по умолчанию и конструктора копирования
	files_tree();
	void GetAllFiles( char* path_to_directory, vector<char*>& buffer, bool add_hidden );
	// рекурсивная функция инициализации дерева
	void Initialize( char* full_path, char* file_name, int& id, int parent_id, files_tree_node& node );
	// рекурсивная функция освобождения ресурсов дерева
	void Dispose( files_tree_node& node );
	// рекурсивная функция получения всех элементов дерева
	void GetSubnodes( files_tree_node root_node, vector<file_system_object>& nodes, bool add_hidden );
	// функция отображающая элемент дерева на структуру объекта фс
	void MapNode( files_tree_node& node, file_system_object& object );
	// конкатенация путей
	char* CreatePath( char* root_path, char* file_path );
	// получение имени файла по пути.
	char* GetFileName( char* path );
public:
	files_tree( char* root_path, bool with_hidden );
	~files_tree();

	void GetNodes( vector<file_system_object>& nodes );
};

#endif