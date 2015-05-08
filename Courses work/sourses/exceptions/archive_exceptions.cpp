#include "archive_exceptions.h"
#include <iostream>

archive_exception::archive_exception( const char* message )
{
	this -> message = message;
}

archive_exception::archive_exception( const char* message, const char* inner_message )
{
	this -> message = message;
	this -> inner_message = inner_message;
}

const char* archive_exception::GetMessage()
{
	return message;
}

const char* archive_exception::GetInnerMessage()
{
	return inner_message;
}