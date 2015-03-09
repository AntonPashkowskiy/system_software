#if !defined( THREADS_SYNCH )
#define THREADS_SYNCH
#include <pthread.h>
#include <vector>
#endif

void* ShowUniqueString( void* arguments );

class ThreadSynch
{
    private:
        std::vector< pthread_t > _threadsID;
        char _commandString[ 50 ];
        bool CheckCommandString();

    public:
        ~ThreadSynch();
        void EnterCommandString();
        bool InitMutex();
        bool Execute();
        void AddThread();
        void KillLastThread();
        void KillAllThreads();
};


