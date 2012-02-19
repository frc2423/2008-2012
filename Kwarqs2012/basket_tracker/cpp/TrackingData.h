

#ifndef __TRACKING_DATA_H
#define __TRACKING_DATA_H

// defines the tracking data that is used by the main python code
struct TrackingData 
{
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
    
    double angle_susan;
    
};


#endif
