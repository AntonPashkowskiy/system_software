#if !defined( FILE_SYSTEM_OBJECT )
#define FILE_SYSTEM_OBJECT
#include "file_type.h"

struct file_system_object
{
	char* full_path;
	char* file_name;
	file_type type;
	int object_id;
	int parent_id;
};

#endif