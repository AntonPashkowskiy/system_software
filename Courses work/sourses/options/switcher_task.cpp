#include "switcher_task.h"
#include "../archivation/archivator.h"
#include "../archivation/masher_archivator.h"
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


using namespace std;

void switcher_task::RunTask( archive_options* options )
{
	if( options == nullptr ) return;

	masher_archivator _archivator;

	switch( options -> requested_operation )
	{
		case CREATE_WITHOUT_COMPRESSING:
		case CREATE_WITH_COMPRESSING:

			if( CheckPermissions( options ) )
			{
				if( options -> comment != nullptr )
				{
					char* path_to_file = CreateFileWithComment( options -> comment );
					(options -> paths).push_back( path_to_file );
				}

				try
				{
					_archivator.RunArchivation( options );
				}
				catch( archive_exception e )
				{
					e.ShowMessage();
				}

				cout << "Archiving completed successfully" << endl;
			}
			else
			{
				cerr << "Archiving is suspended." << endl;
			}

			break;

		case EXTRACTING:

			if( CheckArchiveExisting( options ) )
			{
				if( options -> target_path != nullptr )
				{
					if( !CheckTargetPath( options -> target_path ) )
					{
						cout << "Target path is invalid." << endl;
						cout << "Extracting archive in current directory? [Y/N]: ";
						
						char choise;
						cin >> choise;

						if( choise == 'Y' )
						{
							// target_path это константная строка, её нельзя удалить динамически
							options -> target_path = nullptr;
						}
						else
						{
							return;
						} 
					}
				}

				try
				{
					_archivator.ExtractFiles( options );
				}
				catch( archive_exception e )
				{
					e.ShowMessage();
				}
			}
			else
			{
				cerr << "Archive is not found." << endl;
			}

			break;

		case REMOVING:

			try
			{
				if( CheckArchiveExisting( options ) )
				{
					_archivator.RemoveFiles( options );
				}
				else
				{
					cerr << "Archive is not found." << endl;
				}
			}
			catch( archive_exception e )
			{
				e.ShowMessage();
			}
			
			break;

		case VIEW_TITLE:

			try
			{
				if( CheckArchiveExisting( options ) )
				{
					vector<title_node> title;
					title = _archivator.GetTitle( options );
					PrintTitle( title );
				}
				else
				{
					cerr << "Archive is not found." << endl;
				}
			}
			catch( archive_exception e )
			{
				e.ShowMessage();
			}

			break;

		case CHECK:

			try
			{
				if( CheckArchiveExisting( options ) )
				{
					if( _archivator.ChechIntegrity( options ) )
					{
						cout << "Arсhive is integrite." << endl;
					}
					else
					{
						cerr << "Archive is dameged." << endl;
					}
				}
				else
				{
					cerr << "Archive is not found." << endl;
				}
			}
			catch( archive_exception e )
			{
				e.ShowMessage();
			}

			break;

		default:
			// никогда не произойдёт потому что комманд
			// HELP, COMMENT, UNDEFINED, INCLUDE_HIDDEN_FILES на данном этапе нет
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
		// access проверяет файлы на существование и права чтения
		if( access( paths[ i ], F_OK ) == 0 && access( paths[ i ], R_OK ) == 0 )
		{
			// функция stat собирает статистику о файле указанном в пути
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

	delete statistics;
	return result;
}

void switcher_task::PrintTitle( vector<title_node> title )
{

}

// создание файла с комментарием
char* switcher_task::CreateFileWithComment( char* comment )
{
	char* path = "CommentTempFile.msrc";
	int file_descriptor = open( path, O_CREAT | O_WRONLY | O_TRUNC );

	if( file_descriptor != -1 )
	{
		write( file_descriptor, comment, strlen( comment ) );
	}
	
	return path;
}

// проверка существования архива
bool switcher_task::CheckArchiveExisting( archive_options* options )
{
	return access( options -> target_archive_name, F_OK ) == 0;
}

// проверка существования пути распаковки
bool switcher_task::CheckTargetPath( char* path )
{
	struct stat* statistics = new struct stat;

	if( access( path, F_OK ) == 0 )
	{
		if( stat( path, statistics ) == 0 )
		{
			if( S_ISDIR( statistics -> st_mode ) )
			{
				return true;
			}
		}
	}

	delete statistics;
	return false;
}