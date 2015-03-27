// подключаем список используемый в параметрах
#if !defined( STL_LIST )
#define STL_LIST
#include <vector>
#endif

#if !defined( ARCHIVE_OPTIONS )
#define ARCHIVE_OPTIONS

enum Operation { CREATE_WITHOUT_COMPRESSING,
				 CREATE_WITH_COMPRESSING,
				 EXTRACTING, 
				 REMOVING,
				 CHECK, 
				 VIEW_TITLE, 
				 COMMENT, 
				 HELP, 
				 UNDEFINED };

// опции которые необходимо определить для дальнейшей работы программы
struct archive_options
{
	Operation requested_operation;
	char* target_archive_name = nullptr;
	char* target_path = nullptr;
	char* comment = nullptr;
	std::vector<char*> paths;
};

#endif