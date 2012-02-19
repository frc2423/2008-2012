
#include <WPILib.h>
#include <cmath>
#include "BasketTracker.h"


// constructor
BasketTracker::BasketTracker() :
    // run the vision task at low priority, otherwise
    // nothing else on the robot can get done!
    m_task( "BasketTracker", (FUNCPTR)BasketTracker::TaskFunction, 200 ),
    m_verbose(false)
{
    // Initialize the tracking data
    m_tracking_data.frame_number = 0;
    m_tracking_data.valid_frames = 0;
    m_tracking_data.target_data_valid = false;
    
    // create a lock to use for synchronization
    m_lock = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
    
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
    
    TrackingData data( m_tracking_data );
    return data;
}

void BasketTracker::SetVerbose( bool verbose )
{
    // not protected by a lock, because we don't care
    m_verbose = true;
}


void BasketTracker::TaskFunction( UINT32 this_pointer )
{
    ((BasketTracker*)this_pointer)->Main();
}

void BasketTracker::Main()
{
    // basket tracking initialization
    
    // Initialize Axis camera
    AxisCamera &camera = AxisCamera::GetInstance("10.24.23.11");
    Wait(3.0);
    
    // Color thresholds in the HSL color space (bright Green)
    Threshold targetThreshold(96,131,136,255,145,179);
    
    // Rectangle dimensions to filter
    ParticleFilterCriteria2 criteria[] = {
            {IMAQ_MT_BOUNDING_RECT_WIDTH, 40, 400, false, false},
            {IMAQ_MT_BOUNDING_RECT_HEIGHT,40, 400, false, false}
    };
    
    // Camera angle in radians 
    double theta = 0.6691;
    
    // Get the analog input from the Sonar module
    AnalogChannel sonar(4);
    
    // this gets copied to a buffer for the python code to use
    TrackingData data;

    {
        Synchronized lock(m_lock);
        
        // initialize the data here from whatever we initialized in
        // the constructor
        data = m_tracking_data;
    }
    
    // run in an infinite loop doing basket tracking stuff until
    // signaled to exit
    
    // while ( true )
    while ( data.frame_number == 0 )
    {
        // Getting images from the camera and color segmenting the reflective tape 
        HSLImage  *hslImage  = camera.GetImage();
        
        // Color thesholding in HSL color space
        BinaryImage *rectanglesSegmented = hslImage->ThresholdHSL(targetThreshold);
        
        // Filtering small objects
        BinaryImage *bigObjectsImage = rectanglesSegmented->RemoveSmallObjects(false, 2); 
        
        // Filling rectangles 
        BinaryImage *convexHullImage = bigObjectsImage->ConvexHull(false); 
        
        // Filtering objects by width and height
        BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 2); 
        
        // Index filtered objects and get statistical analysis report
        vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();
        
        // Geting distance to target from SONAR output voltage values
        float sonar_voltage_out = sonar.GetVoltage();
        
        // Distance estimation to target via SONAR 
        double sonar_voltage_in = 0.0098;
        data.sonar_distance = sonar_voltage_out / sonar_voltage_in;
            
        vector<ParticleAnalysisReport>::iterator i = reports->begin();
        const vector<ParticleAnalysisReport>::iterator end = reports->end();
        
        // always invalidate the data in case we didn't get any targets
        // -> TODO: Add additional logic to get back to the target
        data.target_data_valid = false;
        
            
        for ( ; i != end; ++i ) 
        {
            // if we have at least one report, the data is valid
            data.target_data_valid = true;
            
            data.x = i->center_mass_x;
            data.y = i->center_mass_y;
        
            double bw = i->boundingRect.width;
            double temp2 = 2.0 * bw * tan(theta/2.0);
            data.distance  = (double) 640*22 / temp2;
            
            if (m_verbose)
            {
                printf("coordinate of target (x,y): %i , %i \n", i->center_mass_x, i->center_mass_y);    
                printf("distance to target (in inches) %f \n", data.distance);
                printf("distance to target (SONAR based in inches): %f \n", data.sonar_distance);    
                printf("difference between the distance estimated from Sonar and vision system: %f \n", data.sonar_distance - data.distance );
            }
        }
        
        if (m_verbose)
        {
            printf("---------- \n");
        }

        delete hslImage;
        delete rectanglesSegmented;
        delete bigObjectsImage;
        delete convexHullImage;
        delete filteredImage;
        delete reports;
        
        //
        // Every time we process a frame, increment the count so we can do
        // things like calculate frame rate, determine if new data arrived.. 
        //
        
        data.frame_number += 1;
        
        //
        // This might be useful at some point.. 
        //
        
        if (data.target_data_valid)
            data.valid_frames += 1;
    
        //
        // This part transfers data to the user of this class
        //
        
        { 
            Synchronized lock( m_lock );
            
            // just copy everything over, it's not too big yet
            m_tracking_data = data;
        }
    }
}

