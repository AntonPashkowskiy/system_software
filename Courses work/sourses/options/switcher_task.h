#if !defined( SWITCHER_TASK )
#define SWITCHER_TASK
#if !defined( STL_VECTOR )
#define STL_VECTOR
#include <vector>
#endif
#include "archive_options.h"
#include "../exceptions/archive_exceptions.h"
#include "../archivation/archivator.h"
#include "../archivation/masher_archivator.h"
#include "../archivation/header.h"

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
	// проверяет есть ли архив по заданному пути
	static bool CheckArchiveExisting( archive_options* options );
	// целевой путь должен быть путём директории.
	static bool CheckTargetPath( char* path );
	// вывод справки
	static void ShowHelp();
	// вывод заголовка архива
	static void PrintHeader( archive_header header );
public:
	static void RunTask( archive_options* options );
};

#endif
