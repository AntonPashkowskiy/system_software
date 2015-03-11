#include "exception.h"
#include <iostream>

exception::exception( char* message )
{
	_message = message;
	_called_function = NULL;
}

exception::exception( char* message, char* called_function )
{
	_message = message;
	_called_function = called_function;
}

void exception::Log( int descriptor, char* message, char* called_function )
{
	//function for courses work
}

void exception::Message()
{
	if( _called_function != NULL )
	{
		std::cout << _called_function << ": " << std::endl;
	}

	if( _message != NULL )
	{
		std::cout << message << std::endl;
	}
}