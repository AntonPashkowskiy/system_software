#include "argument_handler.h"
#include <iostream>

using namespace std;

void argument_handler::ShowHelp()
{
	cout << "FLAGS:" << endl
		 << "flag: -cr     create archive without compressing." << endl
		 << "flag: -crc    create archive with compressing." << endl
		 << "flag: -rmf	   remove files from archive." << endl
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
		 << "example: masher -check archive.msr" << endl;
}

archive_options* argument_handler::ProcessArgument( int argc, char** argv )
{
	if( argc > 1 )
	{
		archive_options* options;
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
				cerr << "Error. Invalid target archive name. Apply --help." << endl;
				return nullptr;
			}
		}

		switch( operation )
		{
			case CREATE_WITHOUT_COMPRESSING:
			case CREATE_WITH_COMPRESSING:

				if( argc > 4 )
				{
					for( int i = 3; i < argc; i++ )
					{
						// для отлова возможного флага комментария
						Operation check_flag = GetFlag( argv[ i ] );
							
						if( check_flag == UNDEFINED && CheckPath( argv[ i ] ) )
						{
							(options -> paths).push_back( argv[ i ] );
						}
						else if( check_flag == COMMENT && argc == i + 2 )
						{
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
					cerr << "Error. Not enought arguments for creating operation. Apply --help." << endl;
					return nullptr;
				}

				break;

			case EXTRACTING:

				// если есть целевая директория
				if( argc == 4 )
				{
					if( CheckPath( argv[ 3 ] ) )
					{
						options -> target_path = argv[ 3 ];
					}
				}
				else
				{
					cerr << "Error. Invalid argumetns count. Apply --help." << endl;
					return nullptr;
				}

				break;

			case REMOVING:

				// если предполагаемых путей один или больше
				if( argc >= 4 )
				{
					for( int i = 4; i < argc; i++ )
					{
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
					cerr << "Error. Not enought arguments for removing operation. Apply --help." << endl;
					return nullptr;
				}

		
			case VIEW:

				return options;

			case UNDEFINED: 
				cerr << "Error. Undefined flag: " << argv[ 1 ] << ". Apply --help." << endl;
				return nullptr;
		}
	}
	else
	{
		cout << "error: Arguments not found. Apply --help." << endl;
		return nullptr;
	}
}