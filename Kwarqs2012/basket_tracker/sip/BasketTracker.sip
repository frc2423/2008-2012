//
// SIP module definition for Kwarqs 2012 basket tracker
//

%Module _basket_tracker

%DefaultEncoding "UTF-8"


struct TrackingData {
    %TypeHeaderCode
    #include "TrackingData.h"
    %End
    
    double x;
    double y;
	double angularVelocity;
	bool validData;
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
        
    private:
    
        // no copy or assign
        BasketTracker( const BasketTracker &other);
};
