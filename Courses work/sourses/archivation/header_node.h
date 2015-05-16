#if !defined( ARCHIVE_HEADER_NODE )
#define ARCHIVE_HEADER_NODE

const int max_name_length = 255;

/*
	Структура представляющая собой
	элемент заголовка архива. Состоит из имени файла,
	размера файла, типа ( регулярный файл или директория ),
	байт предсталяющий собой алгоритм сжатия файла 
	и информацию о дереве для возможности воссоздания дерева.
*/

struct header_node
{
	char file_name[ max_name_length ];
	unsigned long long file_size = 0;
	char file_type;
	char compression_type;
	int file_id = 0;
	int file_parent_id = 0;
	int tree_id = 0;
};

#endif