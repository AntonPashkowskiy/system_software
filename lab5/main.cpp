#include "exception.h"
#include "thread_direction.h" 
#include <dlfcn.h>
#include <iostream>

int main()
{
	void* library = NULL;

	if( library = dlopen( "", RTLD_NOW ) == NULL )
	{
		std::cerr << "Library was not loaded." << endl;
		return -1;
	}

	try
	{

	}
	catch( exception ex )
	{
		ex.Message();
	}

	dlclose( library );
	return 0;
}