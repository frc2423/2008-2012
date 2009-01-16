#include "WPILib.h"
#include <cmath>

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	DriverStation * ds;
	
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	
	AnalogChannel encoder;

	Gyro gyro;
	Servo servo;
	I2C *lcd;
	
	Accelerometer accelerometer;

	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		encoder(7),
		gyro(1),
		servo(9),
		accelerometer(5)
	{
		GetWatchdog().SetExpiration(100);
		printf("Entered OperatorControl\n");
		
		// rate for the gyro
		gyro.SetSensitivity(0.007);
		
		ds = DriverStation::GetInstance();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		/*
		GetWatchdog().SetEnabled(false);
		myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
		*/
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		printf("Entered OperatorControl\n");
		
		double time = GetTime();
		GetWatchdog().SetEnabled(true);
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			
			double angle = 360 - fabs(fmod(gyro.GetAngle(), 360));
			
			if (angle > 180)
			{
				angle = angle - 180;
			}
			
			servo.SetAngle(angle);
			
			// only print messages every once in awhile, don't overload the system
			if (GetTime() - time > 0.25)
			{
				//printf("Gyro: %f, servo: %f\r", gyro.GetAngle(), angle);
				//printf("voltage: %f\r", encoder.GetVoltage());
				//printf("JoyX: %5.3f JoyY: %5.3f Trig: %s\r", 
				//stick.GetX(), stick.GetY(), 
		    	//stick.GetTrigger() == true ? "1" : "0");
				
				// convert g's to units of meters/sec
				//float acc = accelerometer.GetAcceleration() * (1/9.81);
				
				//float velocity  = (acc * acc) / 2;
				
				//printf("Acc: %f Velocity: %f\r", acc, velocity);
				
				printf("1: %f 2: %f 3: %f\r", 
						ds->GetAnalogIn(1),
						ds->GetAnalogIn(2),
						ds->GetAnalogIn(3));
				
				time = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

