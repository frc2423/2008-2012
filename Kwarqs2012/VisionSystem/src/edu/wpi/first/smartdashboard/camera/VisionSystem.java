package edu.wpi.first.wpilibj.examples;

import edu.wpi.first.smartdashboard.camera.WPILaptopCameraExtension;
import edu.wpi.first.smartdashboard.robot.Robot;
import edu.wpi.first.wpijavacv.WPIBinaryImage;
import edu.wpi.first.wpijavacv.WPIColor;
import edu.wpi.first.wpijavacv.WPIColorImage;
import edu.wpi.first.wpijavacv.WPIContour;
import edu.wpi.first.wpijavacv.WPIImage;
import edu.wpi.first.wpijavacv.WPIPoint;
import edu.wpi.first.wpijavacv.WPIPolygon;
import edu.wpi.first.wpilibj.networking.NetworkTable;
import java.util.ArrayList;

/**
 *
 * @author Greg Granito
 */
public class VisionSystem extends WPILaptopCameraExtension {
    public static final String NAME = "Laptop Camera Square Tracker";

    NetworkTable table = NetworkTable.getTable("camera");
    WPIColor targetColor = new WPIColor(255, 0, 0);
    /*
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
            //min_threshold = cvScalar( 60, 120,  220, 0);
            //max_threshold = cvScalar(140, 150, 255, 0);
            min_threshold = cvScalar( 55, 125, 250, 0);
            max_threshold = cvScalar( 62, 130, 255, 0);
            
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
            //table.putInt("x", this.x);
            //table.putInt("y", this.y);
            table.putBoolean("target_data_valid", this.target_data_valid);
            table.putDouble("angle_susan", this.angle_susan);
            table.putDouble("distance", i);
            //table.putDouble("sonar_distance", i);
            //table.putDouble("angle_susan", i);
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
    */
    
    
    @Override
    public WPIImage processImage(WPIColorImage rawImage) {
        
        double IMAGE_WIDTH = 640.0;
        double AXIS_CAMERA_VIEW_ANGLE = Math.PI * 38.33 / 180.0;

        WPIBinaryImage blueBin = rawImage.getBlueChannel().getThresholdInverted(60);
        WPIBinaryImage greenBin = rawImage.getGreenChannel().getThresholdInverted(60);
        WPIBinaryImage redBin = rawImage.getRedChannel().getThresholdInverted(60);

        WPIBinaryImage finalBin = blueBin.getAnd(redBin).getAnd(greenBin);

        finalBin.erode(2);
        finalBin.dilate(6);

        WPIContour[] contours = finalBin.findContours();

        ArrayList<WPIPolygon> polygons = new ArrayList<WPIPolygon>();

        for(WPIContour c : contours){
            double ratio = ((double)c.getHeight()) / ((double)c.getWidth());
            if(ratio < 1.5 && ratio> 0.75){
                polygons.add(c.approxPolygon(45));
            }
        }

        ArrayList<WPIPolygon> possiblePolygons = new ArrayList<WPIPolygon>();

        for(WPIPolygon p : polygons){
            if(p.isConvex() && p.getNumVertices() == 4){
                possiblePolygons.add(p);
            }else{
                rawImage.drawPolygon(p, WPIColor.CYAN, 5);
                
            }
        }

        WPIPolygon square = null;
        int squareArea = 0;

        for(WPIPolygon p : possiblePolygons){
            rawImage.drawPolygon(p, WPIColor.GREEN, 5);
            for(WPIPolygon q : possiblePolygons){
                if(p == q) continue;

               int pCenterX = (p.getX() + (p.getWidth()/2));
               
               int qCenterX = q.getX() + (q.getWidth()/2);
               
               int pCenterY = (p.getY() + (p.getHeight()/2));

               int qCenterY = q.getY() + (q.getHeight()/2);

               rawImage.drawPoint(new WPIPoint(pCenterX, pCenterY), targetColor, 5);
               rawImage.drawPoint(new WPIPoint(qCenterX, qCenterY), targetColor, 5);
               
                // filter only the targets that have their centers not that far from each other 
                // and get the bigest square 
                if(Math.abs(pCenterX - qCenterX) < 20 &&
                    Math.abs(pCenterY - qCenterY) < 20){
                    int pArea = Math.abs(p.getArea());
                    int qArea = Math.abs(q.getArea());
                    // choosing the biger target (closest)
                    if(pArea > qArea){
                        square = p;
                        squareArea = pArea;
                    }else{
                        square = q;
                        squareArea = qArea;
                    }
                    break;
                }
            }
        }

        if(square != null){
            // x coordinate of the center
            double center_x = square.getX() + (square.getWidth()/2);
            double  square_width = square.getWidth();
            // x in a relative coordinate system (-1, 1)
            //x = (2 * (x/rawImage.getWidth())) - 1;
            double angle_susan = (IMAGE_WIDTH / 2.0 - center_x) * AXIS_CAMERA_VIEW_ANGLE / IMAGE_WIDTH;
            double distance = (IMAGE_WIDTH * 22.0) / (2.0 * square_width * Math.tan(AXIS_CAMERA_VIEW_ANGLE/2.0));
            //double area = ((double)squareArea) /  ((double)(rawImage.getWidth() * rawImage.getHeight()));
            
            synchronized(table) {
            table.beginTransaction();
                table.putBoolean("found", true);
                table.putDouble("angle_susan", angle_susan);
                table.putDouble("distance", distance);
            table.endTransaction();
            }

            Robot.getTable().putBoolean("found", true);
            Robot.getTable().putDouble("distance", distance);
            Robot.getTable().putDouble("angle_susan", angle_susan);
            rawImage.drawPolygon(square, targetColor, 7);
        }else{
            table.putBoolean("found", false);
            Robot.getTable().putBoolean("found", false);
        }

        return rawImage;
    }
}

