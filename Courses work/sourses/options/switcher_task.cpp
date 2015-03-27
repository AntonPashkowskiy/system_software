#include "switcher_task.h"
#include <iostream>
#include <new>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

void switcher_task::RunTask( archive_options* options )
{
	if( options == nullptr ) return;

	switch( options -> requested_operation )
	{
		case CREATE_WITHOUT_COMPRESSING:
		case CREATE_WITH_COMPRESSING:

			if( CheckPermissions( options ) )
			{
				if( options -> comment != nullptr )
				{
					char* path_to_file = CreateFileWithComment( char* comment );
					(options -> paths).push_back( path_to_file );
				}

				// функция по запуску архивации (обработка исключений)
				cout << "Archiving completed successfully" << endl;
				return;
			}
			else
			{
				cerr << "Archiving is suspended." << endl;
				return;
			}

		case EXTRACTING:

			if( CheckArchiveExisting( options ) )
			{
				if( options -> target_path != nullptr )
				{
					if( !CheckTargetPath( options -> target_path )
					{
						cout << "Target path is not found." << endl;
						cout << "Extracting archive in current directory? [Y/N]: ";
						
						char choise;
						cin >> choise;

						if( choise == 'Y' )
						{
							options -> target_path = nullptr;
						}
						else
						{
							return;
						} 
					}
				}

				// запуск функции по распаковке архива
			}
			else
			{
				cerr << "Archive is not found." << endl;
				return;
			}

		case REMOVING:

			// запуск удаления файлов (кидает исключение если нет файлов с таким именем)
			return;

		case VIEW_TITLE:

			// vector<title_node> title = null;
			// запуск функции которая возвращает вектор со структурами title_node;
			// PrintTitle( title );
			return;

		case CHECK:

			// запуск функции проверки целостности, которая считывает 
			// записанный после архивации размер запакованного архива и
			// и сравнивает его с размером на данный момент
			break;

		default:
			// никогда не произойдёт потому что комманд
			// HELP, COMMENT, UNDEFINED нет
			break;
	}
}

bool switcher_task::CheckPermissions( archive_options* options )
{
	int paths_count = (options -> paths).size();
	vector<char*> paths = options -> paths;
	struct stat* statistics = new struct stat;
	bool result = false;

	for( int i = 0; i < paths_count; i++ )
	{
		if( access( paths[ i ], F_OK ) == 0 && access( paths[ i ], R_OK ) == 0 )
		{
			if( stat( paths[ i ], statistics ) == 0 )
			{
				if( S_ISDIR( statistics -> st_mode ) )
				{
					result = true;
				}
				else if( S_ISREG( statistics -> st_mode ) )
				{
					result = true;
				}
				else if( S_ISBLK( statistics -> st_mode ) )
				{
					cout << "Block device can not be archived: " << paths[ i ] << endl;
					result = false;
				}
				else if( S_ISCHR( statistics -> st_mode ) )
				{
					cout << "Character device can not be archived: " << paths[ i ] << endl;
					result = false;
				}
			}
			else
			{
				cout << "Was unable to obtain statistics: " << paths[ i ] << endl;
				result = false;
			}
		}
		else
		{
			cout << "File or directory not found: " << paths[ i ] << endl;
			result = false;
		}
	}

	return result;
}

void switcher_task::PrintTitle( vector<title_node> title )
{

}

char* switcher_task::CreateFileWithComment( char* comment )
{

}

bool switcher_task::CheckArchiveExisting( archive_options* options )
{
	if( access( options -> target_archive_name, F_OK ) == 0 )
	{
		return true;
	}

	return false;
}