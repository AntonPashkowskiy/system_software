#if !defined( WORK_WITH_FILES )
#define WORK_WITH_FILES
#endif

class fileSearcher
{
private:
	char* _search_mask;
public:
	fileSearcher( char* search_mask );
	int GetNextFile();
};