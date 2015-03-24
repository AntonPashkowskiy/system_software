#include "argument_handler.h"
#include <iostream>
#include <string.h>

using namespace std;

void argument_handler::ShowHelp()
{
	cout << "\n ----------------- HELP -----------------\n" << endl
		 << "FLAGS:" << endl
		 << "flag: -cr     create archive without compressing." << endl
		 << "flag: -crc    create archive with compressing." << endl
		 << "flag: -rmf    remove files from archive." << endl
		 << "flag: -ext    extract files from archive." << endl
		 << "flag: -vt     view title." << endl
		 << "flag: -cmt    add а comment to archive."
		 << "flag: -check  check archive integrity.\n" << endl
		 << "EXAMPLES:" << endl
		 << "example: masher -cr archive.msr somedirectory somefiles" << endl
		 << "example: masher -crc archive.msr somedirectory somefiles" << endl
		 << "example: masher -rmf archive.msr filenames" << endl
		 << "example: masher -vt archive.msr" << endl
		 << "example: masher -ext archive.msr target_directory (current by default)" << endl
		 << "example: masher -crc archive.msr somedirectory somefiles -cmt \"Comment\"" << endl
		 << "example: masher -check archive.msr\n" << endl;
}

archive_options* argument_handler::ProcessArgument( int argc, char** argv )
{
	if( argc > 1 )
	{
		archive_options* options = new archive_options;
		Operation operation = GetFlag( argv[ 1 ] );
		options -> requested_operation = operation;

		if( operation == HELP )
		{
			ShowHelp();
			return nullptr;
		}

		// устанавливаем целевой архив
		if( argc > 2 )
		{
			if( CheckTargetArchName( argv[ 2 ] ) )
			{
				options -> target_archive_name = argv[ 2 ];
			}
			else
			{
				cerr << "Error. Invalid archive name. Apply --help." << endl;
				return nullptr;
			}
		}

		switch( operation )
		{
			case CREATE_WITHOUT_COMPRESSING:
			case CREATE_WITH_COMPRESSING:

				if( argc >= 4 )
				{
					for( int i = 3; i < argc; i++ )
					{
						// для отлова возможного флага комментария
						Operation check_flag = GetFlag( argv[ i ] );
							
						if( check_flag == UNDEFINED && CheckPath( argv[ i ] ) )
						{
							// проверяем аргументы на дублирование.
							int paths_count = (options -> paths).size();
							
							for( int j = 0; j < paths_count; j++ )
							{
								if( strcmp( argv[ i ], (options -> paths)[ j ] ) == 0 )
								{
									cerr << "Error. Path duplication: " << argv[ i ] << endl;
									return nullptr;
								}
							}

							(options -> paths).push_back( argv[ i ] );
						}
						else if( check_flag == COMMENT && argc == i + 2 ) 
						{
							// если комментарий найден а путей никаких нет
							if( (options -> paths).size() == 0 )
							{
								cerr << "Error. Not enought arguments. Apply --help." << endl;
								return nullptr;
							}
							// если таки нашли комментарий - добавляем его текст
							// в опции и выходим - а после комментария хоть rm -rf
							options -> comment = argv[ i + 1 ];
							return options;
						}
						else
						{
							// выводим часть ошибочной комманды
							cerr << "Error. Invalid argument: ";

							for( int j = i; j < argc; j++ )
							{
								cout << argv[ j ] << " ";
							}

							cout << endl;
							return nullptr;
						}
					}

					return options;
				}
				else
				{
					cerr << "Error. Not enought arguments. Apply --help." << endl;
					return nullptr;
				}

			case EXTRACTING:

				// если есть целевая директория
				if( argc == 4 )
				{
					if( CheckPath( argv[ 3 ] ) )
					{
						options -> target_path = argv[ 3 ];
					}
				}

				return options;


			case REMOVING:

				// если предполагаемых путей один или больше
				if( argc >= 4 )
				{
					for( int i = 3; i < argc; i++ )
					{
						// проверяем аргументы на дублирование
						int paths_count = (options -> paths).size();
							
						for( int j = 0; j < paths_count; j++ )
						{
							if( strcmp( argv[ i ], (options -> paths)[ j ] ) == 0 )
							{
								cerr << "Error. Path duplication: " << argv[ i ] << endl;
								return nullptr;
							}
						}

						if( CheckPath( argv[ i ] ) )
						{
							(options -> paths).push_back( argv[ i ] );	
						}
						else
						{
							// выводим часть ошибочной комманды
							cerr << "Error. Invalid argument: ";

							for( int j = i; j < argc; j++ )
							{
								cout << argv[ j ] << " ";
							}

							cerr << endl;
							return nullptr;
						}
					}

					return options;
				}
				else
				{
					cerr << "Error. Not enought arguments. Apply --help." << endl;
					return nullptr;
				}

		
			case VIEW_TITLE:

				if( argc == 3 )
				{
					return options;
				}
				else
				{
					cerr << "Error. To many arguments for this command." << endl;
					return nullptr;
				}

			// никогда не произойдёт, но компилятор ругается
			case HELP:

				return nullptr;

			case COMMENT:

				cerr << "-cmt flag using only with (-cr/-crc) flags. Apply --help." << endl;
				return nullptr;

			case CHECK:

				if( argc == 3 )
				{
					return options;
				}
				else
				{
					cerr << "Error. To many arguments for this command." << endl;
					return nullptr;
				}

			case UNDEFINED:
			 
				cerr << "Error. Undefined flag: " << argv[ 1 ] << ". Apply --help." << endl;
				return nullptr;
		}
	}
	else
	{
		cerr << "Error: Arguments not found. Apply --help." << endl;
		return nullptr;
	}

	// прибиваем варнинг
	return nullptr;
}

Operation argument_handler::GetFlag( char* flag )
{
	if( flag == nullptr ) return UNDEFINED;

	if( strcmp( flag, "--help" ) == 0 ) return HELP;
	if( strcmp( flag, "-crc" ) == 0 )   return CREATE_WITH_COMPRESSING;
	if( strcmp( flag, "-vt" ) == 0 )    return VIEW_TITLE;
	if( strcmp( flag, "-cr" ) == 0 )    return CREATE_WITHOUT_COMPRESSING;
	if( strcmp( flag, "-ext" ) == 0 )   return EXTRACTING;
	if( strcmp( flag, "-rmf" ) == 0 )   return REMOVING;
	if( strcmp( flag, "-cmt" ) == 0 )   return COMMENT;
	if( strcmp( flag, "-check" ) == 0 ) return CHECK;

	return UNDEFINED;
}

bool argument_handler::CheckTargetArchName( char* name )
{
	if( name == nullptr ) return false;

	int length = strlen( name );

	for( int i = 0; i < length; i++ )\
	{
		if( IsDeniedSymbol( name[ i ] ) )
		{
			return false;
		}

		if( name[ i ] == '.' )
		{
			// имя архива не может начинатся с .msr, а после .msr не должно быть других символов
			if( i != 0 && /*strcmp( name + i, ".msr" ) &&*/ length == i + 4  )
			{
				return true;
			}

		}
	}

	return false;
}

bool argument_handler::CheckPath( char* path )
{
	// может когда нибудь тут будет нормальная проверка путей,
	// когда нибудь...
	// но сейчас она считает путём всё что не является флагом
	if( GetFlag( path ) == UNDEFINED )
	{
		return true;
	}

	return false;
}

bool argument_handler::IsDeniedSymbol( char symbol )
{
	// сдесь должен проверятся весь список запрещённых символов в имени файла
	return false;
}

// функция для тестирования возвращённых опций
void argument_handler::Test( archive_options* options )
{
	if( options == nullptr )
	{
		cout << "Is null pointer." << endl;
	}

	switch( options -> requested_operation )
	{
		case CREATE_WITHOUT_COMPRESSING:
			cout << "Operation: CREATE_WITHOUT_COMPRESSING." << endl;
			break;

		case CREATE_WITH_COMPRESSING:
			cout << "Operation: CREATE_WITH_COMPRESSING." << endl;
			break;

		case EXTRACTING:
			cout << "Operation: EXTRACTING." << endl;
			break;

		case REMOVING:
			cout << "Operation: REMOVING." << endl;
			break;

		case CHECK:
			cout << "Operation: CHECK." << endl;
			break;

		case VIEW_TITLE:
			cout << "Operation: VIEW_TITLE." << endl;
			break;

		case COMMENT: 
			cout << "Operation: COMMENT." << endl;
			break;

		case HELP:
			cout << "Operation: HELP." << endl;
			break;

		case UNDEFINED:
			cout << "Operation: UNDEFINED." << endl;
			break;  
	}

	if( options -> target_archive_name != nullptr )
	{
		cout << "Target archive name: " << (options -> target_archive_name) << endl;
	}

	if( options -> target_path != nullptr )
	{
		cout << "Target path: " << (options -> target_path) << endl;
	}

	if( options -> comment != nullptr )
	{
		cout << "Comment: " << (options -> comment) << endl;
	}

	vector<char*> vector = options -> paths;
	int length = vector.size();

	if( length != 0 )
	{
		cout << "Paths: " << endl;

		for( int i = 0; i < length; i++ )
		{
			cout << vector[ i ] << endl; 
		}
	}	

	
}