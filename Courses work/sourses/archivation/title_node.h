#if !defined( ARCHIVE_TITLE_NODE )
#define ARCHIVE_TITLE_NODE

const int max_name_length = 255;

struct title_node
{
	char file_name[ max_name_length ];
	unsigned long long file_length = 0;
	char file_type;
	char compression_type;
	int file_id = 0;
	int file_parent_id = 0;
	int tree_id = 0;
};

#endif