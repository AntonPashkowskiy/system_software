#if !defined( ARCHIVE_TITLE )
#define ARCHIVE_TITLE
#include "title_node.h"

#if !defined( STL_VECTOR )
#define STL_VECTOR
#include <vector>
#endif

struct archive_title
{
	std::vector<title_node> nodes;
	char* comment = nullptr;
};

#endif