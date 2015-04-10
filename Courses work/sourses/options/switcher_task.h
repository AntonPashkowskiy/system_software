#if !defined( SWITCHER_TASK )
#define SWITCHER_TASK
#if !defined( STL_VECTOR )
#define STL_VECTOR
#include <vector>
#endif
#include "../exceptions/archive_exceptions.h"
#include "archive_options.h"
#include "../archivation/title_node.h"

using namespace std;

/*
	Класс необходим как прослойка для конечной обработки опций
	таких как проверка прав доступа и существования путей, 
	выполняет все второстепенные действия ( вроде создания файла с комментарием )
	и запускает исполнение выбранной операции с проверенными опциями.
*/

class switcher_task
{
private:
	static bool CheckPermissions( archive_options* options );
	// проверяет есть ли путь в текущей папке
	static bool CheckArchiveExisting( archive_options* options );
	// целевой путь должен быть путём директории.
	static bool CheckTargetPath( char* path );
	static void PrintTitle( vector<title_node> title );
	// возвращает путь к файлу
	static char* CreateFileWithComment( char* comment );
public:
	static void RunTask( archive_options* options );
};

#endif
