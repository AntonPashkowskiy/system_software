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

const int buffer_size = 8388608;

void masher_archivator::RunArchivation( archive_options* options )
{
	vector<file_system_object> target_files;
	vector<file_tree*> file_trees;

	for ( unsigned int i = 0; i < (options -> paths).size(); i++ )
	{
		file_tree* tree = new file_tree( (options -> paths)[ i ], options -> include_hidden_files, i );
		tree -> GetNodes( target_files );
		file_trees.push_back( tree );
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

	DestroyTrees( file_trees );
}

void masher_archivator::RunExtracting( archive_options* options )
{
	archive_header header;

	try
	{
		header = GetHeader( options );
	}
	catch( archive_exception e )
	{
		throw archive_exception( "Error extraction.", e.GetMessage() );
	}

	int archive_descriptor = open( options -> target_archive_name, O_RDONLY );

	if( archive_descriptor  == -1 )
	{
		throw archive_exception( "Error extraction." );	
	}

	vector<file_system_object> files;
	vector<file_system_object> tree_nodes;
	vector<file_tree*> file_trees;
	file_tree* tree = nullptr;
	char* name = nullptr;

	// делаем текущей директорию указанную в опциях,
	// если в опциях не указана целевая директория 
	// создаём директорию с именем архива без расширения
	if( options -> target_path != nullptr )
	{
		if( chdir( options -> target_path ) != 0 )
		{
			throw archive_exception( "Error extraction." );
		}
	}
	else
	{
		int archive_name_length = strlen( options -> target_archive_name );
		name = new char[ archive_name_length ];
		GetArchiveName( name, options -> target_archive_name );
		int descriptor = mkdir( name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH | S_IWOTH );

		if( descriptor == -1 || chdir( name ) != 0 )
		{
			delete name;
			throw archive_exception( "Error extraction." );
		}

		delete name;
	}

	GetFilesFromHeader( header, files );
	int trees_count = TreesCount( files );
	unsigned int j = 0;

	for( int i = 0; i < trees_count; i++ )
	{
		while( true )
		{
			if( files[ j ].tree_id == i && j < files.size() )
			{
				tree_nodes.push_back( files[ j ] );
				j++;
			}
			else
			{
				break;
			}
		}
	
		tree = new file_tree( tree_nodes );
		tree -> CreateFilesTree();
		file_trees.push_back( tree );
		tree_nodes.clear();
	}

	for( int i = 0; i < trees_count; i++ )
	{
		file_trees[ i ] -> GetNodes( tree_nodes );
	}

	Extract( archive_descriptor, header, tree_nodes );
	DestroyTrees( file_trees );
	FreeFileObjects( files );
	close( archive_descriptor );
}

void masher_archivator::RemoveFiles( archive_options* options )
{
	throw archive_exception( "Remove error." );
}

// ------------------------------------------------------------------------------------

void raise_header_getting_error( int descriptor )
{
	// закрываем архив и выбрасываем исключение.
	close( descriptor );
	throw archive_exception( "Failed to get the header. Read error." );
}

/*
	Кода обработки ошибок здесь больше чем бизнес-логики, что и понятно - 
	проверяется результат работы абсолютно всех системных вызовов. 
	Если этого не делать - это чревато абсолютной билибердой у пользователя. 
*/

archive_header masher_archivator::GetHeader( archive_options* options )
{
	// проверяем целостность архива
	// до этого момента нам известно только то что файл архива существует
	try
	{
		bool is_integrite = CheckIntegrity( options );

		if( !is_integrite )
		{
			throw archive_exception( "Failed to get the header. Archive is damaged." );
		}
	}
	catch( archive_exception e )
	{
		throw archive_exception( "Failed to get the header.", e.GetMessage() );
	}

	char comment_flag;
	char* comment = nullptr;
	int comment_length = 0;
	int bytes_read = 0;
	int archive_descriptor = open( options -> target_archive_name, O_RDONLY );

	if( archive_descriptor == -1 )
	{
		throw archive_exception( "Get header error." );
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
						raise_header_getting_error( archive_descriptor );
					}
				}
				else
				{
					raise_header_getting_error( archive_descriptor );
				}
			}
			else
			{
				raise_header_getting_error( archive_descriptor );
			}
		}
		else if( comment_flag == 'u' && bytes_read == sizeof( char ) )
		{
			offset = position;
		}
		else
		{
			raise_header_getting_error( archive_descriptor );
		}

		bool header_is_commpressed = false;
		int header_size = 0;
		// смещаемся на признак сжатия заголовка и размер заголовка
		offset -= ( sizeof( bool ) + sizeof( int ) );

		if( (position = lseek( archive_descriptor, offset, SEEK_SET )) != -1 )
		{
			// считываем размер заголовка и переменную-признак сжатия
			bytes_read = 0;
			bytes_read += read( archive_descriptor, &header_size, sizeof( int ) );
			bytes_read += read( archive_descriptor, &header_is_commpressed, sizeof( bool ) );

			if( bytes_read != sizeof( int ) + sizeof( bool ) )
			{
				raise_header_getting_error( archive_descriptor );
			}
		}
		else
		{
			raise_header_getting_error( archive_descriptor );
		}

		int hn_count = header_size / sizeof( header_node );
		header_node* header_elements = new header_node[ hn_count ];

		if( header_size >= 0 && !header_is_commpressed )
		{
			// устанавливаем смещение в файле на начало заголовка
			offset -= header_size;
			
			if( (position = lseek( archive_descriptor, offset, SEEK_SET )) != -1 )
			{
				// считываем заголовок
				bytes_read = read( archive_descriptor, header_elements, header_size );
				
				if( bytes_read != header_size )
				{
					raise_header_getting_error( archive_descriptor );
				}
			}
		}
		else if( header_size >= 0 && header_is_commpressed )
		{
			// ЗАГЛУШКА ДЛЯ РАСЖАТИЯ ЗАГОЛОВКА
		}
		else
		{
			raise_header_getting_error( archive_descriptor );
		}

		close( archive_descriptor );
		return CreateHeader( header_elements, hn_count, comment ); 
	}
	else
	{
		close( archive_descriptor );
		throw archive_exception( "Failed to get the header. Read error." ); 
	}
}

archive_header masher_archivator::CreateHeader( header_node* header_elements, int count, char* comment )
{
	archive_header header;

	for( int i = 0; i < count; i ++ )
	{
		header.nodes.push_back( header_elements[ i ] );
	}

	header.comment = comment;
	return header;
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
		throw archive_exception( "Error integrity check." ); 
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
			throw archive_exception( "Error integrity check." );
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

	header_node node;
	vector<header_node> header_nodes;
	size_f total_size = 0;
	
	// проверяем право на чтение архивируемых файлов 
	//и удаляем не прошедшие проверку.
	CheckReadAccess( files );

	for( unsigned int i = 0; i < files.size(); i++ )
	{
		node = WriteFileToArchive( archive_descriptor, files[ i ], compress );
		cout << "File '" << node.file_name << "' is packed." << endl;
		header_nodes.push_back( node );
	}

	for( unsigned int i = 0; i < header_nodes.size(); i++ )
	{
		total_size += header_nodes[ i ].file_size;
	}

	total_size += WriteHeader( archive_descriptor, header_nodes, compress );
	total_size += WriteComment( archive_descriptor, comment );
	total_size += sizeof( size_f );
	total_size += sizeof( unsigned );
	WriteTotalSize( archive_descriptor, total_size );
	unsigned check_sum = CalculateCheckSum( archive_descriptor, total_size - sizeof( unsigned ) );
	WriteCheckSum( archive_descriptor, check_sum );
	close( archive_descriptor );
}

void masher_archivator::GetArchiveName( char* name, char* full_name )
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
	int archive_name_length = strlen( target_archive_name );
	char* archive_name = nullptr;
	char postfix[ 3 ];
	// имя архива без расширения
	char name[ archive_name_length ];
	// длинна имени, 3 символа постфикса и \0
	archive_name = new char[ archive_name_length + 4 ];
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
				GetArchiveName( name, target_archive_name );
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

void masher_archivator::CreateHeaderNode( 
	file_system_object& fs_object, 
	header_node& header_n, 
	size_f file_size, 
	char compression_type )
{
	strcpy( header_n.file_name, fs_object.file_name );
	header_n.file_type = fs_object.type == DIRECTORY ? 'd' : 'r';
	header_n.compression_type = compression_type;
	header_n.file_size = file_size;
	header_n.file_id = fs_object.object_id;
	header_n.file_parent_id = fs_object.parent_id;
	header_n.tree_id = fs_object.tree_id;
}

header_node masher_archivator::WriteFileToArchive( int archive_fd, file_system_object object, bool compress )
{
	header_node header_n;

	if( object.type == DIRECTORY )
	{
		// тип сжатия 'n' означает не сжатый файл
		CreateHeaderNode( object, header_n, 0, 'n' );
		return header_n;
	}

	int file_descriptor = open( object.full_path, O_RDONLY );
	size_f file_size = 0;
	char compression_type = 'n';
	char* buffer = new char[ buffer_size ];
	
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
			bytes_read = read( file_descriptor, buffer, buffer_size );
			bytes_written = write( archive_fd, buffer, bytes_read );
			file_size += bytes_written;

			if( bytes_written != bytes_read )
			{
				delete buffer;
				close( file_descriptor );
				close( archive_fd );
				throw archive_exception( "Archive write error." );
			}

			if( bytes_read < buffer_size )
			{
				break;
			}
		}
	}

	CreateHeaderNode( object, header_n, file_size, compression_type );
	close( file_descriptor );
	delete buffer;
	return header_n;
}

int masher_archivator::WriteHeader( int archive_fd, vector<header_node> header, bool compress )
{
	int header_size = 0;
	unsigned int bytes_written = 0;	

	if( compress )
	{
		//сжатие и запись заголовка
	}
	else
	{
		unsigned int nodes_count = header.size();
		header_node* data = header.data();

		bytes_written = write( archive_fd, data, nodes_count * sizeof( header_node ) );
		header_size += bytes_written;
		
		if( bytes_written != nodes_count * sizeof( header_node ) )
		{
			close( archive_fd );
			throw archive_exception( "Error writing archive. Header damaged." );
		}	
	}
	
	// записываем размер заголовка
	bytes_written = write( archive_fd, &header_size, sizeof( int ) );
	bytes_written += write( archive_fd, &compress, sizeof( bool ) );

	if( bytes_written != (sizeof( int ) + sizeof( bool )) )
	{
		close( archive_fd );
		throw archive_exception( "Error writing archive. header damaged." );
	}

	header_size += bytes_written;
	return header_size;
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

	char *buffer = new char[ buffer_size ];
	int bytes_read = 0;
	size_f checked_size = 0;
	bool quit = false;
	CRC32 crc;

	while( true )
	{
		bytes_read = read( descriptor, buffer, buffer_size );

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

void masher_archivator::DestroyTrees( vector<file_tree*>& trees )
{
	for( unsigned int i = 0; i < trees.size(); i++ )
	{
		delete trees[ i ];
	}
}

//------------------------------------------------------------------------------------

void masher_archivator::GetFilesFromHeader( archive_header& header, vector<file_system_object>& files )
{
	file_system_object object;

	for( unsigned int i = 0; i < header.nodes.size(); i++ )
	{
		object = CreateFileObject( header.nodes[ i ] );
		files.push_back( object );
	}
}

file_system_object masher_archivator::CreateFileObject( header_node& node )
{
	file_system_object object;

	int file_name_length = strlen( node.file_name );
	object.file_name = new char[ file_name_length + 1 ];
	strcpy( object.file_name, node.file_name );

	object.type = node.file_type == 'd' ? DIRECTORY : REGULAR;
	object.object_id = node.file_id;
	object.parent_id = node.file_parent_id;
	object.tree_id = node.tree_id;

	return object;
}


int masher_archivator::TreesCount( vector<file_system_object>& files )
{
	int count = 0;
	int current_tree_id = -1;

	for( unsigned int i = 0; i < files.size(); i++ )
	{
		if( files[ i ].tree_id != current_tree_id )
		{
			count ++;
			current_tree_id = files[ i ].tree_id;
		}
	}

	return count;
}

void masher_archivator::FreeFileObjects( vector<file_system_object>& files )
{
	for( unsigned int i = 0; i < files.size(); i++ )
	{
		delete files[ i ].file_name;
	}

	files.clear();
}

void masher_archivator::Extract( 
	int archive_descriptor, 
	archive_header header, 
	vector<file_system_object>& files )
{
	char* buffer = new char[ buffer_size ];
	int file_descriptor = 0;
	size_f bytes_read = 0;
	size_f bytes_written = 0;
	size_f left_to_read = 0;
	size_f offset = 0;
	size_f bf_size = buffer_size;
	bool error = false;

	for( unsigned int i = 0; i < header.nodes.size(); i++ )
	{
		if( files[ i ].type == DIRECTORY )
		{
			continue;
		}

		file_descriptor = open( files[ i ].full_path, O_WRONLY );
		left_to_read = header.nodes[ i ].file_size;

		if( file_descriptor == -1 )
		{
			offset += header.nodes[ i ].file_size;
			cerr << "Failed to extract the " << files[ i ].file_name << " file." << endl;
			close( file_descriptor );
			continue;
		}

		if( lseek( archive_descriptor, offset, SEEK_SET ) == -1 )
		{
			offset += header.nodes[ i ].file_size; 
			cerr << "Failed to extract the " << files[ i ].file_name << " file." << endl;
			close( file_descriptor );
			continue;
		}

		while( left_to_read != 0 )
		{
			if( bf_size > left_to_read )
			{
				bytes_read = read( archive_descriptor, buffer, left_to_read );

				if( bytes_read != left_to_read )
				{
					error = true;
					break;
				}

				left_to_read -= bytes_read;
			}
			else
			{
				bytes_read = read( archive_descriptor, buffer, bf_size );

				if( bytes_read != bf_size )
				{
					error = true;
					break;
				}

				left_to_read -= bytes_read;
			}

			if( header.nodes[ i ].compression_type == 'n' )
			{
				bytes_written = write( file_descriptor, buffer, bytes_read );

				if( bytes_written != bytes_read )
				{
					error = true;
					break;
				}  
			}
			else
			{
				//РАСЖАТИЕ БУФЕРА
			}
		}

		if( error )
		{
			cerr << "Failed to extract the " << files[ i ].file_name << " file." << endl;
		}

		offset += header.nodes[ i ].file_size;
		close( file_descriptor );
	}

	delete buffer;
}