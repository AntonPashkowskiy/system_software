#include "archive_exceptions.h"
#include <iostream>

exception::exception( char* message )
{
	this -> message = message;
	this -> sourse = NULL;
}

exception::exception( char* sourse, char* message )
{
	this -> message = message;
	this -> sourse = sourse;
}

void exception::ShowMessage()
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