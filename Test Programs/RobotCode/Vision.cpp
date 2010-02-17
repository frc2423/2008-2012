/*
 * This implements the vision mode.
 * 
 */


#include "Vision.h"
#include "WPILib.h"

Vision::Vision(RobotResources& resources):
	m_resources(resources),
	pidOutput(m_resources.myRobot),
	turnController( 0.1, // P
				0.00, // I
				0.5, // D
				&m_resources.gyro, // source
				&pidOutput, // output
				0.005), // period
	camera(AxisCamera::GetInstance())
{	
	/**
	 * Set up the PID controller with some parameters that should be pretty
	 * close for most kitbot robots.
	 */
	printf("Initializing PIDController\n");
	turnController.SetInputRange(-360.0, 360.0);
	turnController.SetOutputRange(-0.6, 0.6);
	turnController.SetTolerance(1.0 / 90.0 * 100);
	turnController.Disable();

	// Create and set up a camera instance. first wait for the camera to start
	// if the robot was just powered on. This gives the camera time to boot.
	printf("Setting camera parameters\n");
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteCompression(20);
	camera.WriteBrightness(0);

	// set sensitivity for the 2010 kit m_resources.gyro
	m_resources.gyro.SetSensitivity(0.007);

	// loop getting images from the camera and finding targets
	printf("Starting operator control loop\n");
}

void Vision::enable()
{
	turnController.Enable();
}

void Vision::run()
{
	// if there's a fresh and we're at the previous target heading then
	// get a camera image and process it
	if (camera.IsFreshImage())
	{
		// get the m_resources.gyro heading that goes with this image
		double gyroAngle = m_resources.gyro.PIDGet();

		// get the camera image
		HSLImage *image = camera.GetImage();

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
			dds.sendVisionData(0.0, m_resources.gyro.GetAngle(), 0.0, 0.0, targets);
			if (targets.size() == 0)
				printf("No target found\n\n");
			else
				printf("No valid targets found, best score: %f ", targets[0].m_score);
		}
		else {
			// We have some targets.
			// set the new PID heading setpoint to the first target in the list
			double horizontalAngle = targets[0].GetHorizontalAngle();
			double setPoint = gyroAngle + horizontalAngle;

			turnController.SetSetpoint(setPoint);
			
			// send dashbaord data for target tracking
			dds.sendVisionData(0.0, m_resources.gyro.GetAngle(), 0.0, targets[0].m_xPos / targets[0].m_xMax, targets);
			printf("Target found %f ", targets[0].m_score);
			// targets[0].Print();
		}
	}

	// send the dashbaord data associated with the I/O ports
	dds.sendIOPortData();
}

void Vision::disable()
{
	turnController.Disable();
}
