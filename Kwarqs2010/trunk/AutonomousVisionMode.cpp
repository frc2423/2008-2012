#include "AutonomousVisionMode.h"
#include "WPILib.h"
#include <math.h>

AutonomousVisionMode::AutonomousVisionMode(RobotResources& resources, Kicker& kicker,int balls):
	m_resources(resources),
	m_kicker(kicker),
	m_balls(balls),
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

void AutonomousVisionMode::OnEnable()
{
	turnController.Enable();
}

void AutonomousVisionMode::Main()
{
	static int case_Number = 1;
	static int balls_kicked = 0;
	
	switch(case_Number)
	{
	case 1:
		//against the wall, moving in a straight line, kicking
		m_resources.myRobot.Drive(-0.5, 0.0);
		// if (photogate.SeesBall()) case_Number = 2;
		break;
	case 2:
		//robot stops to kick ball
		m_resources.myRobot.Drive(0.0, 0.0);
		case_Number = 3;
		break;
	case 3:
		// robot aligns with ball
		FindTarget();
		if (turnController.OnTarget()) case_Number = 4;
		break;
	case 4:
		// robot kicks ball
		m_kicker.Kick();
		balls_kicked++;
		break;
	case 5:
		// robot realigns with next ball
		double error = m_resources.gyro.GetAngle();
		error = error / 180.0;
		m_resources.myRobot.Drive(0.2, error);
		if (fabs(error) <= 1.0) case_Number = 6;
		break;
	case 6:
		//Go back to case 1 if there are more balls to kick
		if(balls_kicked < m_balls) case_Number = 1;
		else case_Number = 7;
		break;
	case 7:
		m_resources.myRobot.Drive(0.0, 0.0);
		break;
	}
}

void AutonomousVisionMode::FindTarget()
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

void AutonomousVisionMode::OnDisable()
{
	turnController.Disable();
}
