

#ifndef __BASKET_TRACKER_H
#define __BASKET_TRACKER_H

#include "TrackingData.h"


// the actual camera tracking functionality
class BasketTracker()
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
        
    private:
    
        // no copy or assign
        BasketTracker( const BasketTracker &other );
        BasketTracker& operator=( const BasketTracker &other );
    
        // function executed by the task
        static TaskFunction( void * this_pointer );
        
        // the main function for the basket tracking main
        void Main();

        // task object
        Task m_task;
    
        // lock for synchronization
        SEM_ID m_lock;
    
        // tracking data
        TrackingData m_data;
};



#endif



