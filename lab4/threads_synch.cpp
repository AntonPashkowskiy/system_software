#include "threads_synch.h"
#include <iostream>
#include <string.h>

using namespace std;

ThreadSynch::~ThreadSynch()
{
    KillAllThreads();
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
    return true;
}

bool ThreadSynch::Execute()
{
    return false;
}

void ThreadSynch::AddThread()
{

}

void ThreadSynch::KillLastThread()
{

}

void ThreadSynch::KillAllThreads()
{

}

//---------------- Thread function -----------------

void* ShowUniqueString( void* arguments )
{

}


