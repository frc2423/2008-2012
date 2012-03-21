/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.smartdashboard.camera;

import com.googlecode.javacpp.Loader;
import com.googlecode.javacv.*;
import com.googlecode.javacv.cpp.*;
import static com.googlecode.javacv.cpp.opencv_core.*;
import static com.googlecode.javacv.cpp.opencv_imgproc.*;
import static com.googlecode.javacv.cpp.opencv_calib3d.*;
import static com.googlecode.javacv.cpp.opencv_objdetect.*;

import edu.wpi.first.wpijavacv.WPIColor;
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
        double wheel_speed;
        int i;
        CvScalar min_threshold ;
        CvScalar max_threshold;
        IplConvKernel kernel;
        int kernelSize;
        int KernelAnchorOffset;
        

        
        public TrackingData(){
            // Preload the opencv_objdetect module to work around a known bug.
            Loader.load(opencv_objdetect.class);
    
            frame_number = 0;
            valid_frames = 0;
            target_data_valid = false;
            x = 0;
            y = 0;
            distance = 0;
            sonar_distance = 0.0;
            angle_susan = 0.0;
            wheel_speed = 0.0;
            i = 0;
            // this defines the max and min thesholds for the color images
            min_threshold = cvScalar( 60, 120,  220, 0);
            max_threshold = cvScalar(140, 150, 255, 0);
            kernelSize = 15;
            KernelAnchorOffset = 8;
            kernel = cvCreateStructuringElementEx(kernelSize, kernelSize,KernelAnchorOffset, KernelAnchorOffset,CV_SHAPE_RECT, null);

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
        
        // Geting the image from the camera
        IplImage rgb = IplImage.createFrom(rawImage.getBufferedImage());
        IplImage hls = cvCreateImage(cvGetSize(rgb), 8, 3);
        
        // Converting to HLS colorspace
        cvCvtColor(rgb, hls, CV_BGR2HLS);
        
        // Color Thresholding on the  HLS color space
        IplImage hls_t = cvCreateImage(cvGetSize(rgb), 8, 1);
        cvInRangeS(hls, trackingData.min_threshold, trackingData.max_threshold, hls_t);
        
        // Morphomological Maths dilatation with structuring element defined in the init
        IplImage hls_t_d = cvCreateImage(cvGetSize(rgb), 8,1);
        cvDilate(hls_t, hls_t_d, trackingData.kernel, 1);
        
        //TODO: remove small objects (erosion maybe)
        
        //Find Contours of the multiple rectangles
        CvSeq contours = new CvSeq(null);
        CvMemStorage storage = CvMemStorage.create();
        cvFindContours(hls_t_d, storage, contours, Loader.sizeof(CvContour.class), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        
         while (contours != null && !contours.isNull()) {
                if (contours.elem_size() > 0) {
                    CvSeq points = cvApproxPoly(contours, Loader.sizeof(CvContour.class),
                            storage, CV_POLY_APPROX_DP, 12, 0); //cvContourPerimeter(contours)*0.02
                    //System.out.println(points());
                    cvDrawContours(hls_t_d, points, CvScalar.BLUE, CvScalar.BLUE, -1, 1, CV_AA);
                }        
            contours = contours.h_next();
         }
        // send the resultant image to rawImage which should be displayed
        rawImage = new WPIColorImage(hls_t_d);
        
        // TODO: seems like the garbage collector is not releasing the memory for some reason...
        
        //cvReleaseImage(rgb);
        //cvReleaseImage(hls);
        //cvReleaseImage(hls_t);
        //cvReleaseImage(hls_t_d);
        
        trackingData.PutTrackingData(table);
        
        // Sending rawImage to SmartDashboard
        return super.processImage(rawImage);
    }

}