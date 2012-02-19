#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Vision/HSLImage.h"
#include "Vision/BinaryImage.h"
#include "math.h"

#include "Target.h"
#include "DashboardDataSender.h"

class RobotDemo : public SimpleRobot {
	
	DashboardDataSender *dds;

public:
	RobotDemo(void) {
		
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
		
		// Get the analog input from the SOnar module
		AnalogChannel sonar (4);
		
		while (IsOperatorControl())
		{
			// Getting images from the camera and color segmenting the reflective tape 
			HSLImage  *HSLImage  = camera.GetImage();
			
			// Color thesholding in HSL color space
			BinaryImage *rectanglesSegmented = HSLImage->ThresholdHSL(targetThreshold);
			
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
			double sonar_distance = sonar_voltage_out / sonar_voltage_in;
			
			for (unsigned i = 0; i < reports->size(); i++) {
				
				ParticleAnalysisReport *r = &(reports->at(i));
				double bw = r->boundingRect.width;
				double temp2 = 2.0 * bw * tan(theta/2.0);
				double vision_distance = (double) 640*22 / temp2;
				printf("coordinate of target %i (x,y): %i , %i \n", i, r->center_mass_x, r->center_mass_y);	
				printf("distance to target (ininches) %f \n", vision_distance);
				printf("distance to target (SONAR based in inches): %f \n", sonar_distance);	
				printf("difference between the distance estimated from Sonar and vision system: %f \n", sonar_distance - vision_distance );
			}			
			
			printf("---------- \n");
			delete HSLImage;
			delete rectanglesSegmented;
			delete bigObjectsImage;
			delete convexHullImage;
			delete filteredImage;
			delete reports;
			
		}	
	}

	void OperatorControl(void) {
	}
};

START_ROBOT_CLASS(RobotDemo)
;
