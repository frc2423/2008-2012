#include "WPILib.h"
#include <cmath>
#include <VisionAPI.h>

#include "UDPClient.h"



/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	Jaguar leftMotor;
	Jaguar rightMotor;
	
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	
	Gyro gyro;
	
	Accelerometer accelerometerX;
	Accelerometer accelerometerY;
	
	// moving right is positive

public:
	RobotDemo(void):
		leftMotor(1),
		rightMotor(2),
		myRobot(&leftMotor, &rightMotor),
		stick(1),
		gyro(1),
		accelerometerX(3),
		accelerometerY(4)
	{
		GetWatchdog().SetEnabled(false);
		gyro.SetSensitivity(0.007);
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
		
		double time = GetTime(), start = GetTime();
		GetWatchdog().SetEnabled(true);
		
		// allow writing to file system
		Priv_SetWriteFileAllowed(1);
		
		char fname[128];
		sprintf(fname, "accelerationX-%d.out", (int)time );
		
		FILE * fileX = fopen(fname, "w+");
		if (!fileX)
		{
			fprintf(stderr, "File could not be opened\n");
		}
		
		sprintf(fname, "accelerationY-%d.out", (int)time );
		
		FILE * fileY = fopen(fname, "w+");
		if (!fileY)
		{
			fprintf(stderr, "File could not be opened\n");
		}
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			float accX = accelerometerX.GetAcceleration() * 9.81;
			float accY = accelerometerY.GetAcceleration() * 9.81;	
			
			
			// only print messages every once in awhile, don't overload the system
			if (GetTime() - start > 30)
			{
				if (fileX)
				{
					printf("Done writing file.\n");
					fclose(fileX);
					fclose(fileY);
				}
				
				fileX = NULL;
				fileY = NULL;
			}
			else if (GetTime() - time > 0.1)
			{
				//printf("Gyro: %f, servo: %f\r", gyro.GetAngle(), angle);
				//printf("voltage: %f\r", encoder.GetVoltage());
				//printf("JoyX: %5.3f JoyY: %5.3f Trig: %s\r", 
				//stick.GetX(), stick.GetY(), 
		    	//stick.GetTrigger() == true ? "1" : "0");
				
				// convert g's to units of meters/sec	
				
				fwrite((const char *)&accX, 4, 1, fileX);
				fwrite((const char *)&accY, 4, 1, fileY);		
				
				time = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

