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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.smartdashboard.camera.WPICameraExtension;

/*
Code inspired from the SquareTracker writen by Greg Granito

*/



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
            distance = 0;
            angle_susan = 0.0;
            wheel_speed = 0.0;
            i = 0;
        
        }
    
        
     /*public void PutTrackingData(NetworkTable table){

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
            
            try {
                Thread.sleep(50);
            } catch (InterruptedException ex) {
                Logger.getLogger(VisionSystem.class.getName()).log(Level.SEVERE, null, ex);
            }
        } */
    }
    
    public void init() {
        super.init();
        setPreferredSize(new Dimension(100, 100));
        trackingData = new TrackingData();
        
        NetworkTable.setIPAddress("10.24.23.2");
        //table = NetworkTable.getTable(TABLENAME);

    }
    
    @Override
    public WPIImage processImage(WPIColorImage rawImage) {
        double IMAGE_WIDTH = 640.0;
        double AXIS_CAMERA_VIEW_ANGLE = Math.PI * 38.33 / 180.0;

        preProcessedImage = rawImage.getBufferedImage();
        
        // color thresholding on the 3 RGB color planes
        WPIBinaryImage blueBin = rawImage.getBlueChannel().getThresholdInverted(60);
        WPIBinaryImage greenBin = rawImage.getGreenChannel().getThreshold(60);
        WPIBinaryImage redBin = rawImage.getRedChannel().getThresholdInverted(60);


        WPIBinaryImage finalBin = blueBin.getAnd(redBin).getAnd(greenBin);

        // morpho maths erosion that takes out all small particles
        finalBin.erode(2);
        // dilataion of all binary structures to get rid of whoop occlusion
        finalBin.dilate(6);

        // generate contour structures from binay images    
        WPIContour[] contours = finalBin.findContours();

        ArrayList<WPIPolygon> polygons = new ArrayList<WPIPolygon>();

        // Filtering by rectangle ratio (height / width)
        for(WPIContour c : contours){
            double ratio = ((double)c.getHeight()) / ((double)c.getWidth());
            //System.err.println(ratio);
            if(ratio < 2 && ratio> 0.60){
                //System.err.println("found contour");
                polygons.add(c.approxPolygon(45));
            }
        }

        ArrayList<WPIPolygon> possiblePolygons = new ArrayList<WPIPolygon>();

        // filtering polygons by convexity
        for(WPIPolygon p : polygons){
            if(p.isConvex() && p.getNumVertices() == 4){
                possiblePolygons.add(p);
            }else{
                rawImage.drawPolygon(p, WPIColor.CYAN, 1);
            }
        }

        ArrayList<WPIPolygon> insidePolygons = new ArrayList<WPIPolygon>();
        
        // geting rid of internal contours (inside targets)
        for(WPIPolygon p : possiblePolygons){
            for(WPIPolygon q : possiblePolygons){
                if(p == q) continue;
                
                int pCenterX = (p.getX() + (p.getWidth()/2));
                int qCenterX = q.getX() + (q.getWidth()/2);
                int pCenterY = (p.getY() + (p.getHeight()/2));
                int qCenterY = q.getY() + (q.getHeight()/2);
                
                double distance_between_rects = Math.sqrt(Math.pow(pCenterX - qCenterX, 2) + Math.pow(pCenterY - qCenterY, 2));
            
                if(distance_between_rects < 50){
                    if (p.getArea() > q.getArea()) {
                        insidePolygons.add(q);
                        }
                    else {
                        insidePolygons.add(p);
                    }
                }
            }
        }
        
        // remove all internal contours
        possiblePolygons.removeAll(insidePolygons);
        
        // Draw target's centers
        for (WPIPolygon p : possiblePolygons) {
            int pCenterX = (p.getX() + (p.getWidth()/2));
            int pCenterY = (p.getY() + (p.getHeight()/2));
            rawImage.drawPolygon(p, WPIColor.BLUE, 2);
            rawImage.drawPoint(new WPIPoint(pCenterX, pCenterY), WPIColor.BLUE, 2);
        }
        
        double maxArea = 0.0;
        WPIPolygon biggest_rectangle = null;
        
        // searching for the closest rectangle to camera (the one with the biggest area)
        for (WPIPolygon p : possiblePolygons) {
            if (Math.abs(p.getArea()) >= maxArea){
                maxArea = Math.abs(p.getArea());
                biggest_rectangle = p;
            }
        }
        
        // calculating camera angle to center of target (angle_susan) and distance to target 
        if (biggest_rectangle != null){
            rawImage.drawPolygon(biggest_rectangle, WPIColor.BLUE, 2);
            int pCenterX = (biggest_rectangle.getX() + (biggest_rectangle.getWidth()/2));
            int pCenterY = (biggest_rectangle.getY() + (biggest_rectangle.getHeight()/2));

            rawImage.drawPoint(new WPIPoint(pCenterX, pCenterY), WPIColor.BLUE, 2);
   
            trackingData.frame_number = trackingData.frame_number +1;
            
            // x coordinate of the center
            double center_x = biggest_rectangle.getX() + (biggest_rectangle.getWidth()/2);
            double  biggest_rectangle_width = biggest_rectangle.getWidth();
            double angle_susan = (IMAGE_WIDTH / 2.0 - center_x) * AXIS_CAMERA_VIEW_ANGLE / IMAGE_WIDTH;
            double distance = (IMAGE_WIDTH * 22.0) / (2.0 * biggest_rectangle_width * Math.tan(AXIS_CAMERA_VIEW_ANGLE/2.0));
            
            System.err.println(distance);
            System.err.println(angle_susan);
            
            /*
            if (trackingData.frame_number % 15 == 0)  { 
                synchronized(table) {
                table.beginTransaction();

                table.putBoolean("found", true);
                table.putDouble("angle_susan", angle_susan);
                table.putDouble("distance", distance);
                table.endTransaction();
                }
            }*/
            //if (trackingData.frame_number % 50 == 0)  {
                //System.err.println("--------------------------------------");
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
            //}
            rawImage.drawPolygon(biggest_rectangle, targetColor, 3);
            
        } else{
            System.err.println("no rectangle found");
            /*
            if (trackingData.frame_number % 15 == 0)  {
            table.putBoolean("found", false);
            table.putDouble("angle_susan", 0.0);
            table.putDouble("distance", 0.0);
            table.endTransaction();
            } */
            /*
            table.putBoolean("found", false);
            Robot.getTable().putBoolean("found", false);
            Robot.getTable().putDouble("distance", 0.0);
            Robot.getTable().putDouble("angle_susan", 0.0);*/
        }

        return rawImage;
    }
}

