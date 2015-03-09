#include "threads_synch.h"
#include <iostream>

using namespace std;

int main()
{
    ThreadSynch manager;

    manager.EnterCommandString();
    bool mutexIsCreated = manager.InitMutex();

    if( mutexIsCreated )
    {
        if( manager.Execute() == true )
        {
            cout << "Perform successfully completed." << endl;
        }
        else
        {
            cout << "Execution stopped with errors." << endl;
        }
    }
    else
    {
        cout << "Mutex creating error." << endl;
    }

    cout << "press any key for exit..." << endl;
    cin.get();
    return 0;
}
