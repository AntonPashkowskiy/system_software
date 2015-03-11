#if !defined( THREAD_DIRECTION )
#define THREAD_DIRECTION
#include <pthread.h>
#endif

/* 	
	для того что бы размер буффера, тела функций потока
 	и структура аргументов потока была определена только в файле 
 	с определением методов класса 
*/

#if defined( THREAD_DIRECTION_CPP )
const int buffer_size = 4000;

void* writer( void* arguments );
void* reader( void* arguments );

struct threadsArguments
{
	char* buffer;
	void* library;
	pthread_mutex_t* mutex;
};
#endif

class threadDirection
{
private:
	char _buffer[ 4000 ];
	void* _library;
	pthread_mutex_t _mutex;
	pthread_t _writer_id;
	pthread_t _reader_id;

	//accessory function
	void InitMutex();
	threadArgumets* InitArgumets();

public:
	threadDirection( void* library );
	~threadDirection();
	void Start();
};