#if !defined( THREADS_SYNCH )
#define THREADS_SYNCH
#include <pthread.h>
#include <list>
#endif

struct ThreadArgumets
{
    pthread_mutex_t* mutex;
    char* uniqueString;
};

void* ShowUniqueString( void* arguments );

class ThreadSynch
{
    private:
        std::list< pthread_t > _threadsID;
        std::list<char*> _uStringPointers; //указатели на уникальные строчки потоков
        pthread_mutex_t _mutex;
        char _commandString[ 50 ];

        bool CheckCommandString();
        char* CreateUniqueString();
        ThreadArgumets* FormArgumetns();

    public:
        ~ThreadSynch();
        void EnterCommandString();
        bool InitMutex();
        bool Execute();
        bool AddThread();
        void KillLastThread();
        void KillAllThreads();
};




