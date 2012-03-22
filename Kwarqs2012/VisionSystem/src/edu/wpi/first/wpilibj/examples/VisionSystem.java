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

// Sam's stuff
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.logging.Level;
import java.util.logging.Logger;

// TODELETE?
import edu.wpi.first.smartdashboard.camera.WPICameraExtension;

public class VisionSystem extends WPICameraExtension {
    public static final String NAME = "Camera Square Tracker";

    NetworkTable table = NetworkTable.getTable("camera");
    WPIColor targetColor = new WPIColor(255, 0, 0);

    public static final String TABLENAME = "TrackingData";
    private TrackingData trackingData;
    //private NetworkTable table = null;
    private BufferedImage preProcessedImage;
    
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

        
        // angle in degrees that lazy susan must turn to make the target in the image center
        double angle_susan;
        double wheel_speed;
        int i;
        

        
        public TrackingData(){
            // Preload the opencv_objdetect module to work around a known bug.
    
            frame_number = 0;
            valid_frames = 0;
            target_data_valid = false;
            //x = 0;
            //y = 0;
            distance = 0;
            angle_susan = 0.0;
            wheel_speed = 0.0;
            i = 0;
        
        }
    
        
     public void PutTrackingData(NetworkTable table){

            synchronized(table) {
            table.beginTransaction();
      
            table.putInt("frame_number", this.frame_number);
            table.putInt("valid_frames", this.valid_frames);
            //table.putInt("x", this.x);
            //table.putInt("y", this.y);
            table.putBoolean("target_data_valid", this.target_data_valid);
            table.putDouble("angle_susan", this.angle_susan);
            table.putDouble("distance", this.distance);
            table.endTransaction();
            }
            /*
            try {
                Thread.sleep(50);
            } catch (InterruptedException ex) {
                Logger.getLogger(VisionSystem.class.getName()).log(Level.SEVERE, null, ex);
            }*/
        }
    }
    
    public void init() {
        super.init();
        setPreferredSize(new Dimension(100, 100));
        trackingData = new TrackingData();
        
        NetworkTable.setIPAddress("10.24.23.2");
        table = NetworkTable.getTable(TABLENAME);

    }
    
    @Override
    public WPIImage processImage(WPIColorImage rawImage) {
        double IMAGE_WIDTH = 640.0;
        double AXIS_CAMERA_VIEW_ANGLE = Math.PI * 38.33 / 180.0;

        preProcessedImage = rawImage.getBufferedImage();
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
            Robot.getTable().putDouble("distance", 0.0);
            Robot.getTable().putDouble("angle_susan", 0.0);
        }

        return rawImage;
    }
}

