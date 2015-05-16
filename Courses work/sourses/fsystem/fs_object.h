#if !defined( FILE_SYSTEM_OBJECT )
#define FILE_SYSTEM_OBJECT
#include "file_type.h"

/*
	Структура описывающая обьект файловой системы
	и содержащая всю информацию для воссоздания дерева 
	файлов и директорий в файловой системе.
*/

struct file_system_object
{
	char* full_path = nullptr;
	char* file_name = nullptr;
	file_type type;
	int object_id;
	int parent_id;
	int tree_id;
};

#endif