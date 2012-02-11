#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Vision/HSLImage.h"
#include "Vision/BinaryImage.h"

#include "Target.h"
#include "DashboardDataSender.h"

#define MINIMUM_SCORE 0.01


/**
 * Demo that demonstrates tracking with the robot.
 * In Teleop mode the robot will drive using a single joystick plugged into port 1.
 * When the trigger is pressed, the robot will turn to face the circular target.
 * This demo uses the AxisCamera class to grab images. It finds the target and computes
 * the angle to the target from the center of view in the camera image. It then computes
 * a goal correction angle and uses the gyro to turn to that heading. The gyro correction
 * is done using the PID class to drive the robot to the desired heading.
 * The coefficients for the PID loop will need to be tuned to match the CG and weight of
 * your robot as well as the wheel and driving surface types.
 * 
 * As an added bonus, the program also sends robot status information to the LabVIEW sample
 * dashboard as well as camera annotation to show the targets that the robot is currently
 * seeing.
 */
class RobotDemo : public SimpleRobot {
	
	DashboardDataSender *dds;

public:
	RobotDemo(void) {
		// initialize all variables
		
		//dds = new DashboardDataSender();
		
		AxisCamera &camera = AxisCamera::GetInstance("10.24.23.11");
		//camera.WriteResolution(kResolution_160x120);
		//camera.WriteBrightness(0);
		Wait(3.0);
		//StartCameraTask();
		//PCVideoServer pc;
		//pc.Start();
		
		Threshold targetThreshold(5,55,57,164,127,255);
		ParticleFilterCriteria2 criteria[] = {
				{IMAQ_MT_BOUNDING_RECT_WIDTH, 30, 400, false, false},
				{IMAQ_MT_BOUNDING_RECT_HEIGHT,40, 400, false, false}
		};
		
		while (IsOperatorControl())
		//while (True)
		{
			//HSLImage  *HSLImage  = camera.GetImage();
			HSLImage  *HSLImage  = camera.GetImage();
			BinaryImage *rectanglesSegmented = HSLImage->ThresholdHSL(targetThreshold);
			rectanglesSegmented->Write("rectangles_segmented.jpg");
			BinaryImage *bigObjectsImage = rectanglesSegmented->RemoveSmallObjects(false, 2); 
			BinaryImage *convexHullImage = bigObjectsImage->ConvexHull(false); 
			BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 2); 
			vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();
			
			for (unsigned i = 0; i < reports->size(); i++) {
							ParticleAnalysisReport *r = &(reports->at(i));
							printf("image_timeStamp: %d  particle: %d  center_mass_x: %d  center_mass_y: %d \n ", i, r->imageTimestamp, r->center_mass_x, r->center_mass_y);
						}			
			printf("\n");
			delete HSLImage;
			delete rectanglesSegmented;
			delete bigObjectsImage;
			delete convexHullImage;
			delete filteredImage;
			delete reports;
			
			
			
			
			
			//BinaryImage *rectanglesSegmented = HSLImage->ThresholdHSL(targetThreshold);
			//rectanglesSegmented->Write("rectangles_segmented.jpg");
			//HSLImage->Write("HSLImage.jpg");
			//printf("saved file in rectangles_segmented.jpg \n");
			
			//delete HSLImage;
			//delete rectanglesSegmented;
			//delete rectanglesSegmented;
			//MonoImage *monoImage = rawImage->GetLuminancePlane();
			//Image     *image     = monoImage->GetImaqImage();
			//int workingImageWidth  = monoImage->GetWidth();
			//int workingImageHeight = monoImage->GetHeight();
			//delete monoImage;
			//delete image;
		}	
	}

	/**
	 * Main test program in operator control period.
	 * Aquires images from the camera and determines if they match FRC targets.
	 */
	void OperatorControl(void) {
	}
};

START_ROBOT_CLASS(RobotDemo)
;

