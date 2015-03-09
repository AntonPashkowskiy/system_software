#include "threads_synch.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

//убиваем все процессы и уничтожаем мьютекс
//далее освобождаем память выделенную под строчки

ThreadSynch::~ThreadSynch()
{
    KillAllThreads();
    pthread_mutex_destroy( &_mutex );

    while( _uStringPointers.size() != 0 )
    {
        delete [] _uStringPointers.back();
        _uStringPointers.pop_back();
    }
}

//String contents one-symbol characters:
//a - add thread
//r - remove thread
//q - quit

void ThreadSynch::EnterCommandString()
{
    cout << "Command string: ";

    while( true )
    {
        cin.getline( _commandString, sizeof( _commandString ) );

        if( !CheckCommandString() )
        {
            cout << "There are errors in command string. Try enter again: ";
        }
        else
        {
            break;
        }
    }
}

bool ThreadSynch::CheckCommandString()
{
    int stringLength = strlen( _commandString );

    for( int i = 0; i < stringLength; i++ )
    {
        if( _commandString[ i ] != 'a' &&
            _commandString[ i ] != 'r' &&
            _commandString[ i ] != 'q' )
        {
            return false;
        }
    }

    return true;
}

bool ThreadSynch::InitMutex()
{
    if( pthread_mutex_init( &_mutex, NULL ) != 0 )
    {
        return false;
    }

    return true;
}

bool ThreadSynch::Execute()
{
    return false;
}

bool ThreadSynch::AddThread()
{
    ThreadArgumets* arguments = FormArgumetns();
    pthread_t threadID;

    if( arguments == NULL )
    {
        return false;
    }

    int check = pthread_create( &threadID, NULL, ShowUniqueString, arguments );

    if( check != 0 )
    {
        return false;
    }

    _threadsID.push_back( threadID );
    return true;
}

ThreadArgumets* ThreadSynch::FormArgumetns()
{
    ThreadArgumets* arguments = new ThreadArgumets;

    arguments -> mutex = &_mutex;
    arguments -> uniqueString = CreateUniqueString();

    if( arguments -> uniqueString == NULL )
    {
        return NULL;
    }

    return arguments;
}

char* ThreadSynch::CreateUniqueString()
{
    char* uniqueString = new char[ 30 ];
    char* number = new char[ 4 ];

    strcpy( uniqueString, "I'm thread number " );
    sprintf( number, "%d", _threadsID.size() );
    strcat( uniqueString, number );

    //добавляем указатель в список.
    _uStringPointers.push_back( uniqueString );

    delete [] number;
    return uniqueString;
}

void ThreadSynch::KillLastThread()
{
    pthread_mutex_lock( &_mutex );

    if( _threadsID.size() != 0 )
    {
        pthread_t lastThread = _threadsID.back();

        if (pthread_kill( lastThread, 0) != ESRCH )
        {
            // посылаем последнему потоку в списке
            // служебный сигнал, который ее завершает
            pthread_cancel( lastThread );

            // закрываем дескриптор thread_child */
            pthread_join( lastThread, 0x00);
        }

        _threadsID.pop_back();

        //освобождаем строчку которую использовал поток
        char* stringPointer = _uStringPointers.back();
        _uStringPointers.pop_back();
        delete [] stringPointer;
    }

    pthread_mutex_unlock( &_mutex );
}

void ThreadSynch::KillAllThreads()
{
    while( _threadsID.size() != 0 )
    {
        KillLastThread();
    }
}

//---------------- Thread function -----------------

void* ShowUniqueString( void* arguments )
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0x00);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0x00);

    ThreadArgumets* args = (ThreadArgumets*)argumetns;
    int stringLength = 0;

    if( args -> uniqueString != NULL && args -> mutex != NULL )
    {
        stringLength = strlen( args -> uniqueString );
    }
    else
    {
        return NULL;
    }

    while( true )
    {
        pthread_mutex_lock( args -> mutex );

        for( int i = 0; i < stringLength; i++ )
        {
            cout << ( args -> uniqueString[ i ] );
        }

        cout << endl;
        pthread_mutex_unlock( args -> mutex );
        sleep( 1 );
    }

}


