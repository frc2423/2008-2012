//
// SIP module definition for Kwarqs 2012 basket tracker
//

%Module _basket_tracker

%DefaultEncoding "UTF-8"


struct TrackingData {
    %TypeHeaderCode
    #include "TrackingData.h"
    %End
    
    // this is incremented each time a frame is processed by the
    // basket tracking code
    int frame_number;
    
    // number of frames that a target has been found
    int valid_frames;
    
    // if this is not true, there is NOT valid data here and it
    // should be ignored
    bool target_data_valid;
    
    // coordinate of target (x,y)
    int x;
    int y;
    
    // distance to target (in inches)
    double distance;
    
    // distance to target according to the sonar (in inches)
    // -> this is always valid, but might not actually be the 
    // distance to the target
    double sonar_distance;
};


class BasketTracker {
    %TypeHeaderCode
    #include "BasketTracker.h"
    %End
    
    public:
    
        // constructor
        BasketTracker() /NewThread/;
        
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
        BasketTracker( const BasketTracker &other);
};
