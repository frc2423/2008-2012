#include "WPILib.h"
#include <WebDMA/WebDMA.h>

#include <limits>
#undef min
#undef max

class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	WebDMA webdma;
	DoubleProxy m_gyroAngle;
	Gyro gyro;

public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		gyro(1)
	{
		GetWatchdog().SetExpiration(0.1);
		
		gyro.SetSensitivity(0.007);
		
		m_gyroAngle = webdma.CreateDoubleProxy("Coordinate System", "gyro",
				DoubleProxyFlags()
					.default_value(0)
					.minval(std::numeric_limits<double>::min())
					.maxval(std::numeric_limits<double>::max())
					.step(1)
					.readonly()
			);
		
		webdma.Enable("80", "/www");
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
			m_gyroAngle = gyro.GetAngle();
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

