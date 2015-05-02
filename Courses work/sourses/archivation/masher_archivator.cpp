#include "masher_archivator.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

const int buffer_length = 10000;

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
		throw archive_exception( "Run archivation error.", e );
	}
}

void masher_archivator::ExtractFiles( archive_options* options )
{
	throw archive_exception( "Extracting error." );
}

void masher_archivator::RemoveFiles( archive_options* options )
{
	throw archive_exception( "Removing error." );
}

std::vector<title_node> masher_archivator::GetTitle( archive_options* options )
{
	throw archive_exception( "Title view error." );
}

bool masher_archivator::ChechIntegrity( archive_options* options )
{
	throw archive_exception( "Checking integrity error." );
}

// ------------------------------------------------------------------------

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
		throw new archive_exception( "The archive file is not created." );
	}

	title_node node;
	vector<title_node> title;
	size_f total_size = 0;
	
	// проверяем право на чтение архивируемых файлов 
	//и удаляем не прошедшие проверку.
	CheckReadAccess( files );

	for( unsigned int i = 0; i < files.size(); i++ )
	{
		node = WriteFileInArchive( archive_descriptor, files[ i ], compress );
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
	WriteTotalSize( archive_descriptor, total_size );
	close( archive_descriptor );
}

// функция получения имени архива без расширения
void get_archive_name( char* name, char* full_name )
{
	int length = strlen( full_name );

	for( int i = 0; i < length; i++ )
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

	int archive_descriptor = open( archive_name, O_CREAT | O_WRONLY, 
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

title_node masher_archivator::WriteFileInArchive( int archive_fd, file_system_object object, bool compress )
{
	title_node title_n;

	if( object.type == DIRECTORY )
	{
		// тип сжатия 'n' означает не сжатый файл
		create_title_node( object, title_n, 0, 'n' );
		return title_n;
	}

	int file_descriptor = open( object.full_path, O_RDONLY );
	char* buffer = new char[ buffer_length ];
	size_f file_length = 0;
	char compression_type = 'n';
	
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
				close( file_descriptor );
				close( archive_fd );
				throw archive_exception( "Error writing archive" );
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