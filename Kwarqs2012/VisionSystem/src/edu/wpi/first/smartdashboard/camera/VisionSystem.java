/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.smartdashboard.camera;

import edu.wpi.first.wpijavacv.WPIColorImage;
import edu.wpi.first.wpijavacv.WPIImage;
import edu.wpi.first.wpilibj.networking.NetworkTable;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Greg Granito
 */
public class VisionSystem extends WPICameraExtension {

    public static final String NAME = "Vision System";
    public static final String TABLENAME = "TrackingData";
    private TrackingData trackingData;
    private NetworkTable table;
    
    private class TrackingData
    {
        // this is incremented each time a frame is processed by the
        // basket tracking code
        int frame_number;

        // number of frames that a target has been found
        int valid_frames;

        // if this is not true, there is NOT valid data here and it
        // should be ignored
        boolean target_data_valid;

        // coordinate of target (x,y)
        int x;
        int y;

        // distance to target (in inches)
        double distance;

        // distance to target according to the sonar (in inches)
        // -> this is always valid, but might not actually be the 
        // distance to the target
        double sonar_distance;

        // angle in degrees that lazy susan must turn to make the target in the image center
        double angle_susan;
        int i;
        
        public TrackingData(){
           
            frame_number = 0;
            valid_frames = 0;
            target_data_valid = false;
            x = 0;
            y = 0;
            distance = 0;
            sonar_distance = 0.0;
            angle_susan = 0.0;
            i = 0;
            
        }
        
        public void PutTrackingData(NetworkTable table){
            i++;
            if (i > 1024)
                i = 0;
            table.beginTransaction();
      
            table.putInt("frame_number", this.frame_number);
            table.putInt("valid_frames", this.valid_frames);
            table.putInt("x", this.x);
            table.putInt("y", this.y);
            table.putBoolean("target_data_valid", this.target_data_valid);
            table.putDouble("angle", this.angle_susan);
            table.putDouble("distance", i);
            table.putDouble("sonar_distance", i);
            table.putDouble("angle_susan", i);
            table.endTransaction();
            try {
                Thread.sleep(50);
            } catch (InterruptedException ex) {
                Logger.getLogger(VisionSystem.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    

    @Override
    public void init() {
        super.init();
        trackingData = new TrackingData();
        NetworkTable.setIPAddress("10.24.23.2");
        table = NetworkTable.getTable(TABLENAME);
    }


    @Override
    public WPIImage processImage(WPIColorImage rawImage) {
        //Process image and send to Network Table
        //TODO: process image code
        //TODO: Set all variables from the tracking data class
        trackingData.angle_susan = 2.65;
        trackingData.PutTrackingData(table);

        return super.processImage(rawImage);
    }

}