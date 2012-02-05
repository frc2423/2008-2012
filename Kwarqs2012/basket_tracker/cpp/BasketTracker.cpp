
#include <WPILib.h>
#include "BasketTracker.h"


// constructor
BasketTracker::BasketTracker()
{
    // TODO: Initialize the tracking data
    
    // create a lock to use for synchronization
    m_lock = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
    
    // create a task that does stuff
    m_task = Task( "BasketTracker", (FUNCPTR)BasketTracker::TaskFn );
    
    // run the vision task at low priority, otherwise
    // nothing else on the robot can get done!
    m_task.SetPriority( 200 );
    
    // start the task, and we're done
    m_task.Start( (UINT32)this );
}

// destructor
BasketTracker::~BasketTracker()
{
    // make sure it's not transferring data.. 
    semTake(m_lock, WAIT_FOREVER);
    
    // kill the task
    m_task.Stop();
    
    // delete the semaphore, and we're done
    semDelete(m_lock);
}

// return a copy of our tracking data to the python program
// -> note that it is a *copy* so that the python code doesn't
//    need to acquire any locks
TrackingData BasketTracker::GetData()
{
    Synchronized lock( m_lock );
    
    TrackingData data( m_data );
    return data;
}


void BasketTracker::TaskFn( UINT32 this_pointer )
{
    ((BasketTracker*)this_pointer)->Main();
}

void BasketTracker::Main()
{
    // run in an infinite loop doing basket tracking stuff until
    // signaled to exit
    
    while ( true )
    {
        // if you need to transfer data to the python code, do the following:
        // (note that it MUST be inside braces!
        
        { 
            Synchronized lock( m_lock );
            
            // you can either set the parameters individually, or perhaps 
            // just keep your own local copy, and then just copy the whole
            // thing when its time to update the data?
            m_data.x = 0.0;
            m_data.y = 1.0;
        }
    }
}


