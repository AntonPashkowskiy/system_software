#include "masher_archivator.h"
#include "CRC32.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

const int buffer_length = 8388608;

void masher_archivator::RunArchivation( archive_options* options )
{
	vector<file_system_object> target_files;
	vector<files_tree*> trees_vector;

	for ( unsigned int i = 0; i < (options -> paths).size(); i++ )
	{
		files_tree* tree = new files_tree( (options -> paths)[ i ], options -> include_hidden_files, i );
		tree -> GetNodes( target_files );
		trees_vector.push_back( tree );
	}

	try
	{
		if( options -> requested_operation == CREATE_WITHOUT_COMPRESSING )
		{
			Archive( options -> target_archive_name, target_files, options -> comment, false );
		}
		else
		{
			Archive( options -> target_archive_name, target_files, options -> comment, true );
		}
	}
	catch( archive_exception e )
	{
		throw archive_exception( "Run archivation error.", e.GetMessage() );
	}
}

void masher_archivator::RunExtracting( archive_options* options )
{
	throw archive_exception( "Extraction error." );
}

void masher_archivator::RemoveFiles( archive_options* options )
{
	throw archive_exception( "Remove error." );
}

// ------------------------------------------------------------------------------------

void raise_title_getting_error( int descriptor )
{
	// закрываем архив и выбрасываем исключение.
	close( descriptor );
	throw archive_exception( "Get title error. Read error." );
}

/*
	Кода обработки ошибок здесь больше чем бизнес-логики, что и понятно - 
	проверяется результат работы абсолютно всех системных вызовов. 
	Если этого не делать - это чревато абсолютной билибердой у пользователя. 
*/

archive_title masher_archivator::GetTitle( archive_options* options )
{
	// проверяем целостность архива
	// до этого момента нам известно только то что файл архива существует
	try
	{
		bool is_integrite = CheckIntegrity( options );

		if( !is_integrite )
		{
			throw archive_exception( "Get title error. Archive is damaged." );
		}
	}
	catch( archive_exception e )
	{
		throw archive_exception( "Get title error.", e.GetMessage() );
	}

	char comment_flag;
	char* comment = nullptr;
	int comment_length = 0;
	int bytes_read = 0;
	int archive_descriptor = open( options -> target_archive_name, O_RDONLY );

	if( archive_descriptor == -1 )
	{
		throw archive_exception( "Get title error." );
	}

	// смещаемся на записанную контрольную сумму, длинну файла, и признак наличия комментария 
	int offset = -( sizeof( char ) + sizeof( size_f ) + sizeof( unsigned ) );
	int position = lseek( archive_descriptor, offset, SEEK_END );
	
	if( position != -1 )
	{
		bytes_read = read( archive_descriptor, &comment_flag, sizeof( char ) );

		if( comment_flag == 'c' && bytes_read == sizeof( char ) )
		{
			// считывание длинны комментария
			offset = position - sizeof( int );
			
			if( (position = lseek( archive_descriptor, offset, SEEK_SET )) != -1 )
			{
				bytes_read = read( archive_descriptor, &comment_length, sizeof( int ) );

				if( comment_length >= 0 && bytes_read == sizeof( int ) )
				{
					// смещение на позицию начала комментария
					offset -= comment_length;
					
					if( (position = lseek( archive_descriptor, offset, SEEK_SET )) != -1)
					{
						// считывание комментария
						comment = new char[ comment_length + 1 ];
						bytes_read = read( archive_descriptor, comment, comment_length );
						comment[ comment_length ] = '\0';
						offset = position;
					}
					else
					{
						raise_title_getting_error( archive_descriptor );
					}
				}
				else
				{
					raise_title_getting_error( archive_descriptor );
				}
			}
			else
			{
				raise_title_getting_error( archive_descriptor );
			}
		}
		else if( comment_flag == 'u' && bytes_read == sizeof( char ) )
		{
			offset = position;
		}
		else
		{
			raise_title_getting_error( archive_descriptor );
		}

		bool title_is_commpressed = false;
		int title_size = 0;
		// смещаемся на признак сжатия заголовка и размер заголовка
		offset -= ( sizeof( bool ) + sizeof( int ) );

		if( (position = lseek( archive_descriptor, offset, SEEK_SET )) != -1 )
		{
			// считываем размер заголовка и переменную-признак сжатия
			bytes_read = 0;
			bytes_read += read( archive_descriptor, &title_size, sizeof( int ) );
			bytes_read += read( archive_descriptor, &title_is_commpressed, sizeof( bool ) );

			if( bytes_read != sizeof( int ) + sizeof( bool ) )
			{
				raise_title_getting_error( archive_descriptor );
			}
		}
		else
		{
			raise_title_getting_error( archive_descriptor );
		}

		int tn_count = title_size / sizeof( title_node );
		title_node* title_elements = new title_node[ tn_count ];

		if( title_size >= 0 && !title_is_commpressed )
		{
			// устанавливаем смещение в файле на начало заголовка
			offset -= title_size;
			
			if( (position = lseek( archive_descriptor, offset, SEEK_SET )) != -1 )
			{
				// считываем заголовок
				bytes_read = read( archive_descriptor, title_elements, title_size );
				
				if( bytes_read != title_size )
				{
					raise_title_getting_error( archive_descriptor );
				}
			}
		}
		else if( title_size >= 0 && title_is_commpressed )
		{
			// ЗАГЛУШКА ДЛЯ РАСЖАТИЯ ЗАГОЛОВКА
		}
		else
		{
			raise_title_getting_error( archive_descriptor );
		}

		close( archive_descriptor );
		return CreateTitle( title_elements, tn_count, comment ); 
	}
	else
	{
		close( archive_descriptor );
		throw archive_exception( "Get title error. Read error." ); 
	}
}

archive_title masher_archivator::CreateTitle( title_node* title_elements, int count, char* comment )
{
	archive_title title;

	for( int i = 0; i < count; i ++ )
	{
		title.nodes.push_back( title_elements[ i ] );
	}

	title.comment = comment;
	return title;
}

// ------------------------------------------------------------------------------------

bool masher_archivator::CheckIntegrity( archive_options* options )
{
	struct stat* statistics = new struct stat;
	size_f archive_length = 0;
	size_f read_length = 0;
	bool result = false;
	int archive_descriptor = open( options -> target_archive_name, O_RDONLY );

	if( archive_descriptor == -1 )
	{
		throw archive_exception( "Check integrity error." ); 
	}

	unsigned rd_check_sum = 0;
	// смещение на записанную клнтрольную сумму и на размер файла
	int offset = - ( sizeof( unsigned ) + sizeof( size_f ) );
	
	if( lseek( archive_descriptor, offset, SEEK_END ) != -1 )
	{
		int bytes_read = read( archive_descriptor, &read_length, sizeof( size_f ) );
		bytes_read += read( archive_descriptor, &rd_check_sum, sizeof( unsigned ) );

		if( bytes_read != sizeof( size_f ) + sizeof( unsigned ) )
		{
			throw archive_exception( "Check integrity error." );
		}

		// cчитаем контрольную сумму файла без учёта контрольной суммы, записанной в конце. 
		unsigned check_sum = CalculateCheckSum( archive_descriptor, read_length - sizeof( unsigned ) );

		if( stat( options -> target_archive_name, statistics ) == 0 )
		{
			archive_length = statistics -> st_size;
		}

		if( read_length == archive_length && check_sum == rd_check_sum )
		{
			result = true;
		}
	}

	close( archive_descriptor );
	delete statistics;
	return result;
}

// ------------------------------------------------------------------------------------

void masher_archivator::Archive(
	char* target_archive_name, 
	vector<file_system_object>& files, 
	char* comment, 
	bool compress 
)
{
	// создаём архив и открываем для записи
	int archive_descriptor = CreateArchiveFile( target_archive_name );
	
	if( archive_descriptor == -1 )
	{
		throw archive_exception( "The archive file is not created." );
	}

	title_node node;
	vector<title_node> title;
	size_f total_size = 0;
	
	// проверяем право на чтение архивируемых файлов 
	//и удаляем не прошедшие проверку.
	CheckReadAccess( files );

	for( unsigned int i = 0; i < files.size(); i++ )
	{
		node = WriteFileToArchive( archive_descriptor, files[ i ], compress );
		cout << "File '" << node.file_name << "' is packed." << endl;
		title.push_back( node );
	}

	for( unsigned int i = 0; i < title.size(); i++ )
	{
		total_size += title[ i ].file_length;
	}

	total_size += WriteTitle( archive_descriptor, title, compress );
	total_size += WriteComment( archive_descriptor, comment );
	total_size += sizeof( size_f );
	total_size += sizeof( unsigned );
	WriteTotalSize( archive_descriptor, total_size );
	unsigned check_sum = CalculateCheckSum( archive_descriptor, total_size - sizeof( unsigned ) );
	WriteCheckSum( archive_descriptor, check_sum );
	close( archive_descriptor );
}

// функция получения имени архива без расширения
void get_archive_name( char* name, char* full_name )
{
	int name_length = strlen( full_name );

	for( int i = 0; i < name_length; i++ )
	{
		if( full_name[ i ] == '.' && strcmp( full_name + i, ".msr" ) == 0 )
		{
			name[ i ] = '\0';
			break;
		}

		name[ i ] = full_name[ i ];
	}
}

int masher_archivator::CreateArchiveFile( char* target_archive_name )
{
	int postfix_number = 0;
	char* archive_name = nullptr;
	char postfix[ 3 ];
	// имя архива без расширения
	// max_name_length определён в title_node.h и равен 255
	char name[ max_name_length ] = { '\0' };

	int length = strlen( target_archive_name );
	// длинна имени, 3 символа постфикса и \0
	archive_name = new char[ length + 4 ];
	strcpy( archive_name, target_archive_name );

	while( true )
	{
		// если архив с таким именем (например archive.msr) существует 
		// делаем - archive_1.msr и т.д.

		if( access( archive_name, F_OK ) == 0 )
		{
			sprintf( postfix, "%d", postfix_number );

			if( strlen( name ) == 0 )
			{
				get_archive_name( name, target_archive_name );
			}

			strcpy( archive_name, name );
			strcat( archive_name, "_" );
			strcat( archive_name, postfix );
			strcat( archive_name, ".msr" );
		}
		else
		{
			break;
		}

		postfix_number ++;
	}

	int archive_descriptor = open( archive_name, O_CREAT | O_RDWR, 
		S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH | S_IWOTH );
	
	delete archive_name;
	return archive_descriptor;
}

void masher_archivator::CheckReadAccess( vector<file_system_object> files )
{
	for( unsigned int i = 0; i < files.size(); i++ )
	{
		if( access( files[ i ].full_path, R_OK ) == -1 )
		{
			cout << "File '" << files[ i ].file_name << "' doesn't have read permission." << endl;
			files.erase( files.begin() + i );
		}
	}
}

void create_title_node( 
	file_system_object& fs_object, 
	title_node& title_n, 
	size_f file_length, 
	char compression_type )
{
	strcpy( title_n.file_name, fs_object.file_name );
	title_n.file_type = fs_object.type == DIRECTORY ? 'd' : 'r';
	title_n.compression_type = compression_type;
	title_n.file_length = file_length;
	title_n.file_id = fs_object.object_id;
	title_n.file_parent_id = fs_object.parent_id;
	title_n.tree_id = fs_object.tree_id;
}

title_node masher_archivator::WriteFileToArchive( int archive_fd, file_system_object object, bool compress )
{
	title_node title_n;

	if( object.type == DIRECTORY )
	{
		// тип сжатия 'n' означает не сжатый файл
		create_title_node( object, title_n, 0, 'n' );
		return title_n;
	}

	int file_descriptor = open( object.full_path, O_RDONLY );
	size_f file_length = 0;
	char compression_type = 'n';
	char* buffer = new char[ buffer_length ];
	
	if( compress )
	{
		//сжатие потока и запись
	}
	else
	{
		int bytes_read = 0;
		int bytes_written = 0;

		while( true )
		{
			bytes_read = read( file_descriptor, buffer, buffer_length );
			bytes_written = write( archive_fd, buffer, bytes_read );
			file_length += bytes_written;

			if( bytes_written != bytes_read )
			{
				delete buffer;
				close( file_descriptor );
				close( archive_fd );
				throw archive_exception( "Archive write error." );
			}

			if( bytes_read < buffer_length )
			{
				break;
			}
		}
	}

	create_title_node( object, title_n, file_length, compression_type );
	close( file_descriptor );
	delete buffer;
	return title_n;
}

int masher_archivator::WriteTitle( int archive_fd, vector<title_node> title, bool compress )
{
	int title_size = 0;
	unsigned int bytes_written = 0;	

	if( compress )
	{
		//сжатие и запись заголовка
	}
	else
	{
		unsigned int nodes_count = title.size();
		title_node* data = title.data();

		bytes_written = write( archive_fd, data, nodes_count * sizeof( title_node ) );
		title_size += bytes_written;
		
		if( bytes_written != nodes_count * sizeof( title_node ) )
		{
			close( archive_fd );
			throw archive_exception( "Error writing archive. Title damaged." );
		}	
	}
	
	// записываем размер заголовка
	bytes_written = write( archive_fd, &title_size, sizeof( int ) );
	bytes_written += write( archive_fd, &compress, sizeof( bool ) );

	if( bytes_written != (sizeof( int ) + sizeof( bool )) )
	{
		close( archive_fd );
		throw archive_exception( "Error writing archive. Title damaged." );
	}

	title_size += bytes_written;
	return title_size;
}

int masher_archivator::WriteComment( int archive_fd, char* comment )
{
	char comment_symbol = 'c';
	char no_comment_symbol = 'u';
	unsigned int bytes_written = 0;

	if( comment != nullptr )
	{
		int length = strlen( comment );
		bytes_written += write( archive_fd, comment, length );
		bytes_written += write( archive_fd, &length, sizeof( int ) );
		bytes_written += write( archive_fd, &comment_symbol, sizeof( char ) );

		if( bytes_written != (sizeof( int ) + sizeof( char ) + length) )
		{
			close( archive_fd );
			throw archive_exception( "Error writing archive. Comment damaged." );
		}
	}
	else
	{
		bytes_written = write( archive_fd, &no_comment_symbol, sizeof( char ) );

		if( bytes_written != sizeof( char ) )
		{
			close( archive_fd );
			throw archive_exception( "Error writing archive. Utility information damaged." );
		}
	}

	return bytes_written;
}

void masher_archivator::WriteTotalSize( int archive_fd, size_f total_size )
{
	write( archive_fd, &total_size, sizeof( size_f ) );
}

unsigned masher_archivator::CalculateCheckSum( int descriptor, size_f file_size )
{
	int position = 0;

	if( (position = lseek( descriptor, 0, SEEK_SET )) == -1 )
	{
		close( descriptor );
		throw archive_exception( "Error calculating the check sum." );
	}

	char *buffer = new char[ buffer_length ];
	int bytes_read = 0;
	size_f checked_size = 0;
	bool quit = false;
	CRC32 crc;

	while( true )
	{
		bytes_read = read( descriptor, buffer, buffer_length );

		if( bytes_read == -1 )
		{
			throw archive_exception( "Error calculating the check sum." );
		}

		if( (checked_size + bytes_read) >= file_size )
		{
			bytes_read = file_size - checked_size;
			quit = true;
			break;
		}

		if( bytes_read > 0 )
		{
			// считаем контрольную сумму
			crc.ProcessBytes( buffer, bytes_read );
			checked_size += bytes_read;
		}

		if( quit )
		{ 
			break;
		}
	}

	delete buffer;
	return crc.GetCheckSumm();
}

void masher_archivator::WriteCheckSum( int archive_fd, unsigned check_sum )
{
	int bytes_written = write( archive_fd, &check_sum, sizeof( unsigned ) );

	if( bytes_written != sizeof( unsigned ) )
	{
		throw archive_exception( "Write error." );
	}
}