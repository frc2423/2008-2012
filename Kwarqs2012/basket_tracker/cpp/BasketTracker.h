

#ifndef __BASKET_TRACKER_H
#define __BASKET_TRACKER_H

#include <wpilib.h>
#include "TrackingData.h"


// the actual camera tracking functionality
class BasketTracker
{
    public:
    
        // constructor
        BasketTracker();
        
        // destructor
        ~BasketTracker();
        
        ///////////////////////////////
        // python API
        ///////////////////////////////
        
        // retrieves current vision tracking data
        TrackingData GetData();
        
        // if verbose is true, the vision code will print 
        // out various information as frames are processed
        void SetVerbose( bool verbose = true );
        
    private:
    
        // no copy or assign
        BasketTracker( const BasketTracker &other );
        BasketTracker& operator=( const BasketTracker &other );
    
        // function executed by the task
        static void TaskFunction( UINT32 this_pointer );
        
        // the main function for the basket tracking main
        void Main();

        // task object
        Task m_task;
    
        // lock for synchronization
        SEM_ID m_lock;
    
        // tracking data
        TrackingData m_tracking_data;
        
        bool m_verbose;
};



#endif



