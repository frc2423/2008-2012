/*
 * This implements the vision mode.
 * 
 */


#include "WPILib.h"
#include "Vision.h"
#include <cmath>


#define VISION_TARGET_ACCURACY 2.5

#define VISION_P 0.25
#define VISION_I 0.0
#define VISION_D 0.5

#define VISION_PERIOD 0.005


Vision::Vision(RobotResources& resources):
	m_resources(resources),
	m_turnController( 
				VISION_P, VISION_I, VISION_D, 
				&m_resources.gyro, // source
				this, // output
				VISION_PERIOD), // period
	m_camera(AxisCamera::GetInstance()),
	
	m_notifier( Vision::TimerFn, this )
{	
	/*
	m_horizontalAngle = m_resources.webdma.CreateIntProxy("Vision", "NumTargets",
		IntProxyFlags().readonly()
	);
	
	m_numTargets = m_resources.webdma.CreateDoubleProxy("Vision", "Horizontal Angle",
		DoubleProxyFlags().readonly()
	);
	
	m_isRobotAligned = m_resources.webdma.CreateBoolProxy("Vision", "Aligned", false );
	*/
	
	/**
	 * Set up the PID controller with some parameters that should be pretty
	 * close for most kitbot robots.
	 */
	printf("Initializing PIDController\n");
	m_turnController.SetInputRange(-360.0, 360.0);
	m_turnController.SetOutputRange(1.0, 1.0);
	m_turnController.SetTolerance(1.0 / 90.0 * 100);
	m_turnController.Disable();

	// Create and set up a camera instance. first wait for the camera to start
	// if the robot was just powered on. This gives the camera time to boot.
	printf("Setting m_camera parameters\n");
	m_camera.WriteResolution(AxisCamera::kResolution_320x240);
	m_camera.WriteCompression(20);
	m_camera.WriteBrightness(0);

	// loop getting images from the camera and finding targets
	m_notifier.StartPeriodic( VISION_PERIOD );
	
	printf("Vision Initialized\n");
}

void Vision::EnableMotorControl()
{
	m_turnController.Enable();
}

void Vision::DisableMotorControl()
{
	m_turnController.Disable();
}


void Vision::TimerFn(void * param)
{
	((Vision*)param)->ProcessVision();
}

void Vision::ProcessVision()
{
	// if there's a fresh and we're at the previous target heading then
	// get a camera image and process it
	if (m_camera.IsFreshImage())
	{
		double horizontalAngle = 0;
	
		// get the m_resources.gyro heading that goes with this image
		double gyroAngle = m_resources.gyro.PIDGet();

		// get the camera image
		HSLImage *image = m_camera.GetImage();

		// find FRC targets in the image
		vector<Target> targets = Target::FindCircularTargets(image);
		delete image;
		
		if (targets.size() == 0 || targets[0].m_score < MINIMUM_SCORE)
		{
			// no targets found. Make sure the first one in the list is 0,0
			// since the dashboard program annotates the first target in green
			// and the others in magenta. With no qualified targets, they'll all
			// be magenta.
			Target nullTarget;
			nullTarget.m_majorRadius = 0.0;
			nullTarget.m_minorRadius = 0.0;
			nullTarget.m_score = 0.0;
			if (targets.size() == 0)
				targets.push_back(nullTarget);
			else
				targets.insert(targets.begin(), nullTarget);
			m_dds.sendVisionData(0.0, m_resources.gyro.GetAngle(), 0.0, 0.0, targets);
			
			/*if (targets.size() == 0)
				printf("No target found\n\n");
			else
				printf("No valid targets found, best score: %f ", targets[0].m_score);
			*/
		}
		else {
			// We have some targets.
			// set the new PID heading setpoint to the first target in the list
			horizontalAngle = targets[0].GetHorizontalAngle();
			double setPoint = gyroAngle + horizontalAngle;

			m_turnController.SetSetpoint(setPoint);
			
			// send dashbaord data for target tracking
			m_dds.sendVisionData(0.0, m_resources.gyro.GetAngle(), 0.0, targets[0].m_xPos / targets[0].m_xMax, targets);
			//printf("Target found %f ", targets[0].m_score);
			// targets[0].Print();
		}
		
		
		// WebDMA output stuff:
		m_horizontalAngle = horizontalAngle;
		m_numTargets = targets.size();
		
		if (targets.size() >= 1 && fabs(m_horizontalAngle) < VISION_TARGET_ACCURACY)
			m_isRobotAligned = true;
		else
			m_isRobotAligned = false;
	}

	// send the dashbaord data associated with the I/O ports
	m_dds.sendIOPortData();
}

bool Vision::IsRobotAligned() const 
{
	// doesn't need a mutex since WebDMA already locks for us
	return m_isRobotAligned;
}

void Vision::PIDWrite(float output)
{
	m_resources.myRobot.ArcadeDrive(0.0, output);
}
