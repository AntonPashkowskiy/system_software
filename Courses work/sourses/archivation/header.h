#if !defined( ARCHIVE_HEADER )
#define ARCHIVE_HEADER
#include "header_node.h"

#if !defined( STL_VECTOR )
#define STL_VECTOR
#include <vector>
#endif

/*
	Структура представляющая собой заголовок архива.
	Состоит из комментария и вектора элементов заголовка.
*/

struct archive_header
{
	std::vector<header_node> nodes;
	char* comment = nullptr;
};

#endif