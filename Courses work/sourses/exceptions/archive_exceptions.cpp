#include "archive_archive_exceptions.h"
#include <iostream>

archive_exception::archive_exception( char* message )
{
	this -> message = message;
	this -> sourse = NULL;
}

archive_exception::archive_exception( char* sourse, char* message )
{
	this -> message = message;
	this -> sourse = sourse;
}

void archive_exception::ShowMessage()
{
	if( message != NULL )
	{
		std::cerr << "Exception message:" << message << std::endl; 
	}

	if( sourse != NULL )
	{
		std::cerr << "Exception sourse: " << sourse << std::endl;
	}
}