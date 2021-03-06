#include "WPILib.h"
#include "CompassDrive.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 


class RobotDemo : public SimpleRobot
{
	Joystick stick1; // only joystick
	Joystick stick2;
	Jaguar leftMotor;
	Jaguar rightMotor;
	CompassDrive drive;
	Gyro gyro;

public:
	RobotDemo():
		stick1(1),
		stick2(2),
		leftMotor(1),
		rightMotor(2),
		drive(&leftMotor, &rightMotor),
		gyro(1)
	{
		GetWatchdog().SetExpiration(100);
		
		// rate for the gyro
		gyro.SetSensitivity(0.007);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		printf("Entered OperatorControl\n");
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			bool trigger = stick1.GetTrigger();
			drive.Direction(trigger);
					
			drive.Drive(
					gyro.GetAngle(), 
					stick1.GetAxis(Joystick::kXAxis), 
					stick1.GetAxis(Joystick::kYAxis)*-1,
					stick2.GetAxis(Joystick::kYAxis)*-1);
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

