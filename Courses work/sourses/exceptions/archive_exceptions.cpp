#include "archive_exceptions.h"
#include <iostream>

archive_exception::archive_exception( const char* message )
{
	this -> message = message;
}

archive_exception::archive_exception( const char* message, archive_exception& inner_exception )
{
	this -> message = message;
	*(this -> inner_exception) = inner_exception;
}

const char* archive_exception::GetMessage()
{
	return message;
}

archive_exception archive_exception::GetInnerException()
{
	if( inner_exception != nullptr )
	{
		return *inner_exception;
	}

	return archive_exception( "" );
}