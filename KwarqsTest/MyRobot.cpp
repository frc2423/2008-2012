#include "WPILib.h"
#include <cmath>
#include <VisionAPI.h>

#include "UDPClient.h"
#include "positionTracking.h"

#define MY_PI 3.14

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
	
	Encoder encoderRight;
	Encoder encoderLeft;
	
	// moving right is positive

public:
	RobotDemo(void):
		leftMotor(1),
		rightMotor(2),
		myRobot(&leftMotor, &rightMotor),
		stick(1),
		gyro(1),
		accelerometerX(3),
		accelerometerY(4),
		encoderRight(3,4, false),
		encoderLeft(5,6,false)
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
		sprintf(fname, "sensors-%d.out", (int)time );
		
		FILE * fSensor = fopen(fname, "w");
		if (!fSensor)
		{
			fprintf(stderr, "%s could not be opened\n", fname);
		}
		fprintf(fSensor,"AccelX VelX PosX AccelY PosY Gyro EncoderLR Encoder RR\n");
		
		
		// reset the accelerometer object and set the acceleromter biases
	    AccelerationReset();
	    AccelerationSetBias( .9511, .6285);
	    
	    //start the encoders
	    encoderLeft.Start();
	    encoderRight.Start();
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			
			
			// only print messages every once in awhile, don't overload the system
			if (GetTime() - start > 30)
			{
				if (fSensor)
				{
					printf("Done writing %s.\n", fname);
					fclose(fSensor);
				}
				
			}
			else if (GetTime() - time > 0.1)
			{
				//printf("Gyro: %f, servo: %f\r", gyro.GetAngle(), angle);
				//printf("voltage: %f\r", encoder.GetVoltage());
				//printf("JoyX: %5.3f JoyY: %5.3f Trig: %s\r", 
				//stick.GetX(), stick.GetY(), 
		    	//stick.GetTrigger() == true ? "1" : "0");
				
				// convert g's to units of meters/sec	
				
				//read the sensor data
				float velX, velY, posX, posY;
				float accX = accelerometerX.GetAcceleration() * 9.81;
				float accY = accelerometerY.GetAcceleration() * 9.81;
				float gyroAngle = gyro.GetAngle()*180/MY_PI;
				
				//update the tracking object
				AccelerationUpdate( accX, accY, .1);
				
				//get the filtered acceleration, velocity and position
				GetAcceleration( &accX, &accY);
				GetVelocity( &velX, &velY );
				GetPosition( &posX, &posY );
				
				//read the encoders
				
				//log the sensor values
				fprintf(fSensor,"%f %f %f %f %f %f %f %f %f\n",
						accX, velX, posX, 
						accY, velY, posY,
						gyroAngle, 
						encoderLeft.Get(), encoderRight.Get());
				
				
				time = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

